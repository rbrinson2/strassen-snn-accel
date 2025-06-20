#include <iostream>
#include <memory>
#include <vector>
#include "VSMM1.h"
#include "verilated.h"
#include <armadillo>

#define SMM_FRAME 12
#define N SMM_FRAME * 10
#define THETA 1
#define ALPHA 1.5
#define NUM_MODULES 9


arma::Mat<int32_t> random_matrix(int rows, int cols){
    arma::Mat<int32_t> mat(rows, cols);

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    std::uniform_int_distribution<int32_t> dist {-10, 10};

    mat.for_each([&](arma::Mat<int32_t>::elem_type &val){ val = dist(mersenne_engine); });

    return mat;
}

void write_A(std::unique_ptr<VSMM1> &smm1, const arma::Mat<int32_t>& a);
void write_B(std::unique_ptr<VSMM1> &smm1, const arma::Mat<int32_t>& b);
void read_C(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> &c);
void print_A(std::unique_ptr<VSMM1> &smm1);
void print_B(std::unique_ptr<VSMM1> &smm1);
void print_C(std::unique_ptr<VSMM1> &smm1);

// Helper functions to reduce redundancy
void initialize_module(std::unique_ptr<VSMM1> &module) {
    module->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    module->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    module->clk = 0;
    module->rst = 0;
    module->load = 0;
    module->sel = 0;
}

void set_control_signals(std::vector<std::unique_ptr<VSMM1>>& modules, 
                        int clk, int rst, int load, int sel) {
    for (auto& module : modules) {
        module->clk = clk;
        module->rst = rst;
        module->load = load;
        module->sel = sel;
    }
}

void toggle_clocks(std::vector<std::unique_ptr<VSMM1>>& modules) {
    for (auto& module : modules) {
        module->clk = !module->clk;
    }
}

void evaluate_modules(std::vector<std::unique_ptr<VSMM1>>& modules) {
    for (auto& module : modules) {
        module->eval();
    }
}

void finalize_modules(std::vector<std::unique_ptr<VSMM1>>& modules) {
    for (auto& module : modules) {
        module->final();
    }
}

// ----------------------------------------------------------------------------------- MAIN ----- //
int main(int argc, char const *argv[]) {
    arma::Mat<int32_t> W_L = random_matrix(N, N);
    arma::Col<int32_t> S_L = arma::randi<arma::Col<int32_t>>(N, arma::distr_param(0,1));
    arma::Col<int32_t> V_L(N, arma::fill::zeros);
    arma::Col<int32_t> C_L = V_L;
    arma::Col<int32_t> B_L = S_L;

    // Create weight slices
    arma::Cube<int32_t> W_slice(4, 4, NUM_MODULES, arma::fill::zeros);

    // Verilator setup
    Verilated::mkdir("logs");
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
    contextp->debug(0);
    contextp->randReset(2);
    contextp->traceEverOn(true);
    contextp->commandArgs(argc, argv);

    // Create and initialize modules
    std::vector<std::unique_ptr<VSMM1>> smm_modules;
    smm_modules.reserve(NUM_MODULES);
    for (int i = 0; i < NUM_MODULES; i++) {
        smm_modules.emplace_back(new VSMM1{contextp.get(), "TOP"});
        initialize_module(smm_modules[i]);
    }

    // Result matrices for intermediate calculations
    std::array<arma::Mat<int32_t>, 3> C_results;
    for (auto& mat : C_results) {
        mat.set_size(4, 4);
        mat.fill(0);
    }

    std::cout << "// ------------------------ Matrix x Vector -------------------------------------- //" << std::endl;



    for (int i = 0; i < SMM_FRAME / N; i++){

        arma::Col<int32_t> accum(12, arma::fill::zeros);

        for (int j =  0; j < SMM_FRAME / N; j++){

            
            for (int slice = 0; slice < NUM_MODULES; slice++) {
                int row_start = ((slice / 3) * 4) + i * 12;
                int col_start = ((slice % 3) * 4) + j * 12;
                W_slice.slice(slice) = W_L.submat(arma::span(row_start, row_start + 3), arma::span(col_start, col_start + 3));
            }
            contextp->time(0);
            set_control_signals(smm_modules, 0, 0, 0, 1);

            // Write data to modules
            for (int i = 0; i < NUM_MODULES; i++) {
                write_A(smm_modules[i], W_slice.slice(i));
                int vec_start = ((i % 3) * 4) + j * 12;
                write_B(smm_modules[i], S_L.subvec(vec_start, vec_start + 3));
            }

            // Simulation loop
            while (contextp->time() < 25) {
                contextp->timeInc(1);
                toggle_clocks(smm_modules);

                // Reset logic
                int rst_val = (contextp->time() > 1 && contextp->time() < 4) ? 1 : 0;
                // Load Logic
                int load_val = (contextp->time() >= 3 && contextp->time() < 6) ? 1 : 0;
                
                for (auto& module : smm_modules) {
                    module->rst = rst_val;
                    module->load = load_val;
                }

                evaluate_modules(smm_modules);
            }

            // Process results for each section (0-3, 4-7, 8-11)
            for (int section = 0; section < 3; section++) {
                // Read results from 3 modules for this section
                for (int mod_offset = 0; mod_offset < 3; mod_offset++) {
                    int module_idx = section * 3 + mod_offset;
                    read_C(smm_modules[module_idx], C_results[mod_offset]);
                }
                
                // Update V for this section
                int vec_start = section * 4;
                accum.subvec(vec_start, vec_start + 3) += C_results[0].col(0) + C_results[1].col(0) + C_results[2].col(0);
            }
        }

        // Set accumulation to output vector
        V_L.subvec(i*SMM_FRAME, i*SMM_FRAME + SMM_FRAME - 1) = accum; 

    }


    // Update C and V
    C_L = C_L + W_L * S_L;


    // Print and update states
    std::cout << "\tI:  |  C:  |  B:  |  V:  |  S:" << std::endl;
    std::cout << "\t-----------------------" << std::endl;
    for (int i = 0; i < N; i++) {
        std::cout << "\t " << i << "  |  " << C_L(i) << "  |  " << B_L(i) << "   |  " << V_L(i) << "   |  " << S_L(i) << std::endl;

        // Update C and B
        if (C_L(i) > THETA) {
            C_L(i) = 0;
            B_L(i) = 1;
        } else {
            B_L(i) = 0;
        }

        // Update V and S
        if (V_L(i) > THETA) {
            V_L(i) = 0;
            S_L(i) = 1;
        } else {
            S_L(i) = 0;
        }
    }

    finalize_modules(smm_modules);
    return 0;
}

void write_A(std::unique_ptr<VSMM1> &smm1, const arma::Mat<int32_t>& a) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            smm1->A[i * 4 + j] = a(i, j);
        }
    }
}

void print_A(std::unique_ptr<VSMM1> &smm1) {
    std::cout << "A: " << std::endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << "\t" << smm1->A[i * 4 + j] << " ";
        }
        std::cout << std::endl;
    }
}

void write_B(std::unique_ptr<VSMM1> &smm1, const arma::Mat<int32_t>& b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if ((i * 4 + j) % 4 == 0)
                smm1->B[i * 4 + j] = b(i);
            else
                smm1->B[i * 4 + j] = 0;
        }
    }
}

void print_B(std::unique_ptr<VSMM1> &smm1) {
    std::cout << "B: " << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "\t" << smm1->B[i * 4] << " ";
        std::cout << std::endl;
    }
}

void print_C(std::unique_ptr<VSMM1> &smm1) {
    std::cout << "C2: " << std::endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << "\t" << smm1->C_out[i * 4 + j] << " ";
        }
        std::cout << std::endl;
    }
}

void read_C(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> &c) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            c(i, j) = smm1->C_out[i * 4 + j];
        }
    }
}