
#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>
#include "verilated.h"
#include "VTop.h"

void A_Loading_Logic(std::unique_ptr<VTop> &t, std::vector<std::vector<uint32_t>> &a);
void B_Loading_Logic(std::unique_ptr<VTop> &t, std::vector<std::vector<uint32_t>> &b);
void MAC(std::vector<std::vector<uint32_t>> &a, std::vector<std::vector<uint32_t>> &b, std::vector<uint32_t> &c);

int main(int argc, char const *argv[])
{
    char finished = 0;

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};  // Generates random integers
    std::uniform_int_distribution<int> dist {1, 52};
    auto gen = [&](){return dist(mersenne_engine);};

    std::vector<uint32_t> A_rand(10);
    std::vector<uint32_t> B_rand(10);
    std::vector<uint32_t> C, C_gold;

    std::generate(A_rand.begin(), A_rand.end(), gen);
    std::generate(B_rand.begin(), B_rand.end(), gen);

    std::vector<std::vector<uint32_t>> A_m(3, A_rand);
    std::vector<std::vector<uint32_t>> B_m(3, B_rand);

    MAC(A_m, B_m, C_gold);

    Verilated::mkdir("logs");
    const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext}; 

    contextp->debug(0);
    contextp->randReset(2);
    contextp->traceEverOn(true);
    contextp->commandArgs(argc, argv);

    std::unique_ptr<VTop> top{new VTop{contextp.get(), "TOP"}};
    
    top->clk = 0;
    top->rst = 1;
    
    while (contextp->time() < 100) {
        contextp->timeInc(1);

        top->clk = !top->clk;

        if (top->clk ) {
            if (contextp->time() >= 0 && contextp->time() < 4)
                top->rst = 1;
            else {
                top->rst = 0;
                if (A_m.at(0).empty() && B_m.at(0).empty()) {
                    top->finished = 1;
                    A_Loading_Logic(top, A_m);
                    B_Loading_Logic(top, B_m);
                } 
                else {
                    A_Loading_Logic(top, A_m);
                    B_Loading_Logic(top, B_m);
                }
                
                if (top->Write_en){
                    C.push_back(top->C_out);
                }

            }
        }

        top->eval();
    }

    for (int i = 0; i < C.size(); i++){
        std::cout << " " << i << ": " << C.at(i);
    }
    std::cout << std::endl;

    for (int i = 0; i < C_gold.size(); i++){
        std::cout << " " << i << ": " << C_gold.at(i);
        if (C_gold.at(i) != C.at(i)) std::cout << "FAIL!";
    }
    std::cout << std::endl;



    top->final();
    return 0;
}


void A_Loading_Logic(
    std::unique_ptr<VTop> &t,
    std::vector<std::vector<uint32_t>> &a
){
    
    switch (t->A_read_en)
    {
    case 1:
        t->A_in = a.at(0).front();
        if (t->load_out) a.at(0).erase(a.at(0).begin());
        break;
    case 3:
        t->A_in   = a.at(0).front();
        t->A_in_1 = a.at(1).front();
        if (t->load_out){
            a.at(0).erase(a.at(0).begin());
            a.at(1).erase(a.at(1).begin());
        }
        break;
    case 4:
        t->A_in_2 = a.at(2).front();
        if (t->load_out){
            a.at(2).erase(a.at(2).begin());
        }
        break;
    case 6:
        t->A_in_1 = a.at(1).front();
        t->A_in_2 = a.at(2).front();
        if (t->load_out){
            a.at(1).erase(a.at(1).begin());
            a.at(2).erase(a.at(2).begin());
        }
        break;
    case 7:
        t->A_in = a.at(0).front();
        t->A_in_1 = a.at(1).front();
        t->A_in_2 = a.at(2).front();
        if (t->load_out){
            a.at(0).erase(a.at(0).begin());
            a.at(1).erase(a.at(1).begin());
            a.at(2).erase(a.at(2).begin());
        }
        break;
    default:
        break;
    }

}

void B_Loading_Logic(
    std::unique_ptr<VTop> &t, 
    std::vector<std::vector<uint32_t>> &b
){
    switch (t->B_read_en)
    {
    case 1:
        t->B_in = b.at(0).front();
        if (t->load_out) b.at(0).erase(b.at(0).begin());
        break;
    case 3:
        t->B_in   = b.at(0).front();
        t->B_in_1 = b.at(1).front();
        if (t->load_out){
            b.at(0).erase(b.at(0).begin());
            b.at(1).erase(b.at(1).begin());
        }
        break;
    case 4:
        t->B_in_2 = b.at(2).front();
        if (t->load_out){
            b.at(2).erase(b.at(2).begin());
        }
        break;
    case 6:
        t->B_in_1 = b.at(1).front();
        t->B_in_2 = b.at(2).front();
        if (t->load_out){
            b.at(1).erase(b.at(1).begin());
            b.at(2).erase(b.at(2).begin());
        }
        break;
    case 7:
        t->B_in   = b.at(0).front();
        t->B_in_1 = b.at(1).front();
        t->B_in_2 = b.at(2).front();
        if (t->load_out){
            b.at(0).erase(b.at(0).begin());
            b.at(1).erase(b.at(1).begin());
            b.at(2).erase(b.at(2).begin());
        }
        break;
    default:
        break;
    }


}


void MAC(std::vector<std::vector<uint32_t>> &a, std::vector<std::vector<uint32_t>> &b, std::vector<uint32_t> &c){
    int sum = 0;

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            sum = 0;
            for (int k = 0; k < 10; k++){
                sum += a[i][k] * b[j][k];
            }
            c.push_back(sum);
        }
    }
    std::cout << std::endl;

}