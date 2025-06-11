#include <iostream>
#include <memory>
#include "VSMM1.h"
#include "verilated.h"
#include <armadillo>

#define N 2
#define M N


void write_A(std::unique_ptr<VSMM1> &smm1, arma::Mat<uint32_t> a);
void print_A(std::unique_ptr<VSMM1> &smm1);
void write_B(std::unique_ptr<VSMM1> &smm1, arma::Mat<uint32_t> b);
void read_C(std::unique_ptr<VSMM1> &smm1, arma::Mat<uint32_t> &c);
void print_B(std::unique_ptr<VSMM1> &smm1);
void print_C(std::unique_ptr<VSMM1> &smm1);

int main(int argc, char const *argv[])
{
    arma::Mat<uint32_t> A = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
    arma::Col<uint32_t> B(4, arma::fill::ones);
    arma::Mat<uint32_t> C(4, 4, arma::fill::zeros);
    arma::Mat<uint32_t> C2(4, 4, arma::fill::zeros);
    arma::Mat<uint32_t> C3(4, 4, arma::fill::zeros);

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    std::uniform_int_distribution<uint32_t> dist {1, 10};

    Verilated::mkdir("logs");
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};

    contextp->debug(0);
    contextp->randReset(2);
    contextp->traceEverOn(true);
    contextp->commandArgs(argc, argv);

    std::unique_ptr<VSMM1> smm1_0{new VSMM1{contextp.get(), "TOP"}};
    std::unique_ptr<VSMM1> smm1_1{new VSMM1{contextp.get(), "TOP"}};

    smm1_0->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_0->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_0->clk = 0;
    smm1_0->rst = 0;
    smm1_0->load = 0;
    smm1_0->sel = 0;


    smm1_1->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_1->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_1->clk = 0;
    smm1_1->rst = 0;
    smm1_1->load = 0;
    smm1_1->sel = 0;

    std::cout << "// ------------------------ Matrix x Vector -------------------------------------- //" << std::endl;
    for (int i = 0; i < 1; i++){
        std::cout << "// ------------------------ Epoch " << i + 1 << " -------------------------------------- //" << std::endl;


        contextp->time(0);
        smm1_0->clk = 0;
        smm1_0->rst = 0;
        smm1_0->sel = 1;

        smm1_1->clk = 0;
        smm1_1->rst = 0;
        smm1_1->sel = 1;

        write_A(smm1_0, A);
        print_A(smm1_0);
        std::cout << std::endl;
        write_B(smm1_0, B); 
        print_B(smm1_0);
        std::cout << std::endl;

        while (contextp->time() < 25)
        {
            contextp->timeInc(1);
            smm1_0->clk = !smm1_0->clk;
            smm1_1->clk = !smm1_1->clk;

            // ---------- Reset Logic ---------- //
            if (contextp->time() > 1 && contextp->time() < 4) {
                smm1_0->rst = 1;
                smm1_1->rst = 1;
            }
            else {
                smm1_0->rst = 0;
                smm1_1->rst = 0;
            }

            if (contextp->time() >=3 && contextp->time() < 6) {
                smm1_0->load = 1;
                smm1_1->load = 1;
            }
            else {
                smm1_0->load = 0;
                smm1_1->load = 0;
            }

            smm1_0->eval();
            smm1_1->eval();

            // VL_PRINTF("[%" PRId64"] clk=%x, rstl=%x, load=%x, C_00=%x\n", contextp->time(), smm1_0->clk, smm1_0->rst, smm1_0->load, smm1_0->C_out[0]);
        }

        C = A * B;
        std::cout << "C1: \n" << C << std::endl;
        read_C(smm1_0, C2);
        std::cout <<"C2: \n" << C2.col(0) << std::endl;
        read_C(smm1_1, C3);
        std::cout <<"C3: \n" << C3.col(0) << std::endl;

        A.for_each([&](arma::Mat<uint32_t>::elem_type &val){ val = dist(mersenne_engine); });
        B.for_each([&](arma::Mat<uint32_t>::elem_type &val){ val = dist(mersenne_engine); });
    }
    

    smm1_0->final();
    smm1_1->final();


    return 0;
}



void write_A(std::unique_ptr<VSMM1> &smm1, arma::Mat<uint32_t> a){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            smm1->A[i * 4 + j] = a(i,j);
        }
    }
}
void print_A(std::unique_ptr<VSMM1> &smm1){
    std::cout << "A: " << std::endl; 
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            std::cout << "\t" << smm1->A[i * 4 + j] << " ";
        }
        std::cout << std::endl;
    }
}
void write_B(std::unique_ptr<VSMM1> &smm1, arma::Mat<uint32_t> b){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if ((i * 4 + j) % 4 == 0)
                smm1->B[i * 4 + j] = b(i);
            else
                smm1->B[i * 4 + j] = 0;
        }
    }
}
void print_B(std::unique_ptr<VSMM1> &smm1){
    std::cout << "B: " << std::endl; 
    for (int i = 0; i < 4; i++){
        std::cout << "\t" << smm1->B[i * 4] << " ";
        std::cout << std::endl;
    }
}

void print_C(std::unique_ptr<VSMM1> &smm1){
    std::cout << "C2: " << std::endl; 
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            std::cout << "\t" << smm1->C_out[i * 4 + j] << " ";
        }
        std::cout << std::endl;
    }
}
void read_C(std::unique_ptr<VSMM1> &smm1, arma::Mat<uint32_t> &c){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            c(i,j) = smm1->C_out[i * 4 + j];
        }
    }
}