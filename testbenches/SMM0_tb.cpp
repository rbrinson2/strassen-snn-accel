#include <iostream>
#include <memory>
#include "VSMM0.h"
#include "verilated.h"
#include <armadillo>



int main(int argc, char const *argv[])
{
    arma::Mat<uint32_t> A = {{1,2}, {3,4}};
    arma::Mat<uint32_t> B = {{4,3}, {2,1}};
    arma::Mat<uint32_t> C(2, 2, arma::fill::zeros);

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    std::uniform_int_distribution<uint32_t> dist {1, 10};

    Verilated::mkdir("logs");
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};

    contextp->debug(0);
    contextp->randReset(2);
    contextp->traceEverOn(true);
    contextp->commandArgs(argc, argv);

    std::unique_ptr<VSMM0> smm0{new VSMM0{contextp.get(), "TOP"}};

    smm0->A = {0, 0, 0, 0};
    smm0->B = {0, 0, 0, 0};
    smm0->clk = 0;
    smm0->rst = 0;

    while (contextp->time() < 10)
    {
        contextp->timeInc(1);
        smm0->clk = !smm0->clk;

        if (contextp->time() > 1 && contextp->time() < 4) smm0->rst = 1;
        else {
            smm0->rst = 0;
            smm0->A = {A(0,0),A(0,1),A(1,0),A(1,1)};
            smm0->B = {B(0,0),B(0,1),B(1,0),B(1,1)};

            C = A * B;
            std::cout << C << std::endl;

            A.for_each([&](arma::Mat<uint32_t>::elem_type &val){ val = dist(mersenne_engine); });

        }

        smm0->eval();
    }

    smm0->final();
    return 0;
}
