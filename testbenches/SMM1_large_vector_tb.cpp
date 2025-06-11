#include <iostream>
#include <memory>
#include "VSMM1.h"
#include "verilated.h"
#include <armadillo>

#define N 2
#define M N
#define THETA 1
#define ALPHA 1.5

arma::Mat<int32_t> weights_walk() {
    arma::Mat<int32_t> w_w(12, 12, arma::fill::zeros);
    w_w(0, 9) = 2;
    w_w(1, 10) = 2;
    w_w(2, 11) = 2;
    w_w(3, 8) = 2;
    w_w(4, 9) = 2;
    w_w(5, 10) = 2;
    w_w(6, 11) = 2;
    w_w(7, 8) = 2;
    w_w(8, 9) = 2;
    w_w(9, 10) = 2;
    w_w(10, 11) = 2;
    w_w(11, 8) = 2;

    return w_w;
}

void write_A(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> a);
void print_A(std::unique_ptr<VSMM1> &smm1);
void write_B(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> b);
void read_C(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> &c);
void print_B(std::unique_ptr<VSMM1> &smm1);
void print_C(std::unique_ptr<VSMM1> &smm1);

int main(int argc, char const *argv[])
{

    arma::Mat<int32_t> W_w = weights_walk();
    arma::Col<int32_t> S(12, arma::fill::zeros);
    S = arma::randi<arma::Col<int32_t>>(12, arma::distr_param(0,1));
    arma::Col<int32_t> V(12, arma::fill::zeros);


    arma::Mat<int32_t> A = W_w;
    arma::Col<int32_t> B = S;
    arma::Col<int32_t> C = V;


    arma::Mat<int32_t> C2(4, 4, arma::fill::zeros);
    arma::Mat<int32_t> C3(4, 4, arma::fill::zeros);
    arma::Mat<int32_t> C4(4, 4, arma::fill::zeros);


    arma::Cube<int32_t> W(4,4,9, arma::fill::zeros);
    W.slice(0) = W_w.submat(arma::span(0,3), arma::span(0,3));
    W.slice(1) = W_w.submat(arma::span(0,3), arma::span(4,7));
    W.slice(2) = W_w.submat(arma::span(0,3), arma::span(8,11));
    
    W.slice(3) = W_w.submat(arma::span(4,7), arma::span(0,3));
    W.slice(4) = W_w.submat(arma::span(4,7), arma::span(4,7));
    W.slice(5) = W_w.submat(arma::span(4,7), arma::span(8,11));
    
    W.slice(6) = W_w.submat(arma::span(8,11), arma::span(0,3));
    W.slice(7) = W_w.submat(arma::span(8,11), arma::span(4,7));
    W.slice(8) = W_w.submat(arma::span(8,11), arma::span(8,11));


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
    std::unique_ptr<VSMM1> smm1_2{new VSMM1{contextp.get(), "TOP"}};

    std::unique_ptr<VSMM1> smm1_3{new VSMM1{contextp.get(), "TOP"}};
    std::unique_ptr<VSMM1> smm1_4{new VSMM1{contextp.get(), "TOP"}};
    std::unique_ptr<VSMM1> smm1_5{new VSMM1{contextp.get(), "TOP"}};

    std::unique_ptr<VSMM1> smm1_6{new VSMM1{contextp.get(), "TOP"}};
    std::unique_ptr<VSMM1> smm1_7{new VSMM1{contextp.get(), "TOP"}};
    std::unique_ptr<VSMM1> smm1_8{new VSMM1{contextp.get(), "TOP"}};

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

    smm1_2->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_2->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_2->clk = 0;
    smm1_2->rst = 0;
    smm1_2->load = 0;
    smm1_2->sel = 0;

    smm1_3->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_3->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_3->clk = 0;
    smm1_3->rst = 0;
    smm1_3->load = 0;
    smm1_3->sel = 0;

    smm1_4->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_4->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_4->clk = 0;
    smm1_4->rst = 0;
    smm1_4->load = 0;
    smm1_4->sel = 0;

    smm1_5->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_5->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_5->clk = 0;
    smm1_5->rst = 0;
    smm1_5->load = 0;
    smm1_5->sel = 0;

    smm1_6->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_6->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_6->clk = 0;
    smm1_6->rst = 0;
    smm1_6->load = 0;
    smm1_6->sel = 0;

    smm1_7->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_7->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_7->clk = 0;
    smm1_7->rst = 0;
    smm1_7->load = 0;
    smm1_7->sel = 0;

    smm1_8->A = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_8->B = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    smm1_8->clk = 0;
    smm1_8->rst = 0;
    smm1_8->load = 0;
    smm1_8->sel = 0;

    std::cout << "// ------------------------ Matrix x Vector -------------------------------------- //" << std::endl;
    for (int i = 0; i < 20; i++){
        std::cout << "// ------------------------ Epoch " << i + 1 << " -------------------------------------- //" << std::endl;


        contextp->time(0);
        smm1_0->clk = 0;
        smm1_0->rst = 0;
        smm1_0->sel = 1;

        smm1_1->clk = 0;
        smm1_1->rst = 0;
        smm1_1->sel = 1;

        smm1_2->clk = 0;
        smm1_2->rst = 0;
        smm1_2->sel = 1;

        smm1_3->clk = 0;
        smm1_3->rst = 0;
        smm1_3->sel = 1;

        smm1_4->clk = 0;
        smm1_4->rst = 0;
        smm1_4->sel = 1;

        smm1_5->clk = 0;
        smm1_5->rst = 0;
        smm1_5->sel = 1;

        smm1_6->clk = 0;
        smm1_6->rst = 0;
        smm1_6->sel = 1;

        smm1_7->clk = 0;
        smm1_7->rst = 0;
        smm1_7->sel = 1;

        smm1_8->clk = 0;
        smm1_8->rst = 0;
        smm1_8->sel = 1;

        write_A(smm1_0, W.slice(0));
        write_B(smm1_0, S.subvec(0,3)); 

        write_A(smm1_1, W.slice(1));
        write_B(smm1_1, S.subvec(4,7)); 

        write_A(smm1_2, W.slice(2));
        write_B(smm1_2, S.subvec(8,11)); 

        write_A(smm1_3, W.slice(3));
        write_B(smm1_3, S.subvec(0,3)); 

        write_A(smm1_4, W.slice(4));
        write_B(smm1_4, S.subvec(4,7)); 

        write_A(smm1_5, W.slice(5));
        write_B(smm1_5, S.subvec(8,11)); 

        write_A(smm1_6, W.slice(6));
        write_B(smm1_6, S.subvec(0,3)); 

        write_A(smm1_7, W.slice(7));
        write_B(smm1_7, S.subvec(4,7)); 

        write_A(smm1_8, W.slice(8));
        write_B(smm1_8, S.subvec(8,11));

        while (contextp->time() < 25)
        {
            contextp->timeInc(1);
            smm1_0->clk = !smm1_0->clk;
            smm1_1->clk = !smm1_1->clk;
            smm1_2->clk = !smm1_2->clk;

            smm1_3->clk = !smm1_3->clk;
            smm1_4->clk = !smm1_4->clk;
            smm1_5->clk = !smm1_5->clk;

            smm1_6->clk = !smm1_6->clk;
            smm1_7->clk = !smm1_7->clk;
            smm1_8->clk = !smm1_8->clk;

            // ---------- Reset Logic ---------- //
            if (contextp->time() > 1 && contextp->time() < 4) {
                smm1_0->rst = 1;
                smm1_1->rst = 1;
                smm1_2->rst = 1;

                smm1_3->rst = 1;
                smm1_4->rst = 1;
                smm1_5->rst = 1;

                smm1_6->rst = 1;
                smm1_7->rst = 1;
                smm1_8->rst = 1;
            }
            else {
                smm1_0->rst = 0;
                smm1_1->rst = 0;
                smm1_2->rst = 0;

                smm1_3->rst = 0;
                smm1_4->rst = 0;
                smm1_5->rst = 0;

                smm1_6->rst = 0;
                smm1_7->rst = 0;
                smm1_8->rst = 0;
            }

            if (contextp->time() >=3 && contextp->time() < 6) {
                smm1_0->load = 1;
                smm1_1->load = 1;
                smm1_2->load = 1;

                smm1_3->load = 1;
                smm1_4->load = 1;
                smm1_5->load = 1;

                smm1_6->load = 1;
                smm1_7->load = 1;
                smm1_8->load = 1;
            }
            else {
                smm1_0->load = 0;
                smm1_1->load = 0;
                smm1_2->load = 0;

                smm1_3->load = 0;
                smm1_4->load = 0;
                smm1_5->load = 0;

                smm1_6->load = 0;
                smm1_7->load = 0;
                smm1_8->load = 0;
            }

            smm1_0->eval();
            smm1_1->eval();
            smm1_2->eval();

            smm1_3->eval();
            smm1_4->eval();
            smm1_5->eval();

            smm1_6->eval();
            smm1_7->eval();
            smm1_8->eval();
            // VL_PRINTF("[%" PRId64"] clk=%x, rstl=%x, load=%x, C_00=%x\n", contextp->time(), smm1_0->clk, smm1_0->rst, smm1_0->load, smm1_0->C_out[0]);
        }

        C = C/ALPHA + W_w * S;
        //std::cout << "C:\n" << C;

        read_C(smm1_0, C2);
        read_C(smm1_1, C3);
        read_C(smm1_2, C4);
        V.subvec(arma::span(0,3)) = V.subvec(0,3)/ALPHA + C2.col(0) + C3.col(0) + C4.col(0);

        read_C(smm1_3, C2);
        read_C(smm1_4, C3);
        read_C(smm1_5, C4);
        V.subvec(arma::span(4,7)) = V.subvec(4,7)/ALPHA + C2.col(0) + C3.col(0) + C4.col(0);

        read_C(smm1_6, C2);
        read_C(smm1_7, C3);
        read_C(smm1_8, C4);
        V.subvec(arma::span(8,11)) = V.subvec(8,11)/ALPHA + C2.col(0) + C3.col(0) + C4.col(0);

        //std::cout << "V:\n" << V;

        std::cout << "\tC:  |  B:  |  V:  |  S:" << std::endl;
        std::cout << "\t-----------------------" << std::endl; 
        for (int i = 0; i < 12; i ++){
            std::cout << "\t " << C(i) << "  |  " << B(i) << "   |  " << V(i) << "   |  " << S(i) <<  std::endl;

            if (C(i) > THETA){
                C(i) = 0;
                B(i) = 1;
            }
            else B(i) = 0;

            if (V(i) > THETA) {
                V(i) = 0;
                S(i) = 1;
            } 
            else S(i) = 0;
        }

    }
    

    smm1_0->final();
    smm1_1->final();
    smm1_2->final();

    smm1_3->final();
    smm1_4->final();
    smm1_5->final();

    smm1_6->final();
    smm1_7->final();
    smm1_8->final();

    return 0;
}



void write_A(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> a){
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
void write_B(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> b){
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
void read_C(std::unique_ptr<VSMM1> &smm1, arma::Mat<int32_t> &c){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            c(i,j) = smm1->C_out[i * 4 + j];
        }
    }
}