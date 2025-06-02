#include <iostream>
#include <memory>
#include "VSMM0.h"
#include "verilated.h"
#include <armadillo>

using namespace arma;


int main(int argc, char const *argv[])
{

    Verilated::mkdir("logs");
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};

    contextp->debug(0);
    contextp->randReset(2);
    contextp->traceEverOn(true);
    contextp->commandArgs(argc, argv);

    std::unique_ptr<VSMM0> smm0{new VSMM0{contextp.get(), "TOP"}};

    smm0->A = {0,0,0,0};

    while (contextp->time() < 10)
    {
        contextp->timeInc(1);

        smm0->A = {1,0,1,0};


        smm0->eval();
    }
    
    smm0->final();
    return 0;
}
