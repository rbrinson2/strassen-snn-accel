#include <iostream>
#include <memory>
#include <vector>
#include "VSMM1.h"
#include "verilated.h"
#include <armadillo>

#define N 2
#define M N
#define THETA 1
#define ALPHA 1.5
#define NUM_MODULES 9

arma::Mat<int32_t> weights_walk() {
    arma::Mat<int32_t> w_w(12, 12, arma::fill::zeros);
    w_w(0, 9) = 2;  w_w(1, 10) = 2; w_w(2, 11) = 2; w_w(3, 8) = 2;
    w_w(4, 9) = 2;  w_w(5, 10) = 2; w_w(6, 11) = 2; w_w(7, 8) = 2;
    w_w(8, 9) = 2;  w_w(9, 10) = 2; w_w(10, 11) = 2; w_w(11, 8) = 2;
    return w_w;
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

int main(int argc, char const *argv[]) {
    arma::Mat<int32_t> W_w = weights_walk();
    arma::Col<int32_t> S = arma::randi<arma::Col<int32_t>>(12, arma::distr_param(0,1));
    arma::Col<int32_t> V(12, arma::fill::zeros);
    arma::Col<int32_t> C = V;
    arma::Col<int32_t> B = S;

    // Create weight slices
    arma::Cube<int32_t> W(4, 4, NUM_MODULES, arma::fill::zeros);
    for (int slice = 0; slice < NUM_MODULES; slice++) {
        int row_start = (slice / 3) * 4;
        int col_start = (slice % 3) * 4;
        W.slice(slice) = W_w.submat(arma::span(row_start, row_start + 3), 
                                   arma::span(col_start, col_start + 3));
    }

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
    
    for (int epoch = 0; epoch < 1; epoch++) {
        contextp->timeprecision(9);
        std::cout << "// ------------------------ Epoch " << epoch + 1 << " -------------------------------------- //" << contextp->timeprecision() << std::endl;

        //contextp->time(0);
        set_control_signals(smm_modules, 0, 0, 0, 1);

        // Write data to modules
        for (int i = 0; i < NUM_MODULES; i++) {
            write_A(smm_modules[i], W.slice(i));
            int vec_start = ((i % 3) * 4);
            write_B(smm_modules[i], S.subvec(vec_start, vec_start + 3));
        }

        // Simulation loop
        while (contextp->time() < 1000) {
            contextp->timeInc(5);
            toggle_clocks(smm_modules);

            // Reset logic
            int rst_val = (contextp->time() > 1 && contextp->time() < 20) ? 1 : 0;
            int load_val = (contextp->time() >= 3 && contextp->time() < 30) ? 1 : 0;
            
            for (auto& module : smm_modules) {
                module->rst = rst_val;
                module->load = load_val;
            }

            evaluate_modules(smm_modules);
        }

        // Update C and V
        C = C / ALPHA + W_w * S;

        // Process results for each section (0-3, 4-7, 8-11)
        for (int section = 0; section < 3; section++) {
            // Read results from 3 modules for this section
            for (int mod_offset = 0; mod_offset < 3; mod_offset++) {
                int module_idx = section * 3 + mod_offset;
                read_C(smm_modules[module_idx], C_results[mod_offset]);
            }
            
            // Update V for this section
            int vec_start = section * 4;
            V.subvec(vec_start, vec_start + 3) = V.subvec(vec_start, vec_start + 3) / ALPHA + 
                                                C_results[0].col(0) + C_results[1].col(0) + C_results[2].col(0);
        }

        // Print and update states
        std::cout << "\tC:  |  B:  |  V:  |  S:" << std::endl;
        std::cout << "\t-----------------------" << std::endl;
        for (int i = 0; i < 12; i++) {
            std::cout << "\t " << C(i) << "  |  " << B(i) << "   |  " << V(i) << "   |  " << S(i) << std::endl;

            // Update C and B
            if (C(i) > THETA) {
                C(i) = 0;
                B(i) = 1;
            } else {
                B(i) = 0;
            }

            // Update V and S
            if (V(i) > THETA) {
                V(i) = 0;
                S(i) = 1;
            } else {
                S(i) = 0;
            }
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