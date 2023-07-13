#include <iostream>
#include "orderbook/Init.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Incorrect number of arguments for orderbook, terminating..." << '\n';
        return -1;
    }
    std::cout << "Orderbook starting..." << std::endl;
    Init(argv[1]);
    std::cout << "Orderbook finished, exiting..." << std::endl;
    return 0;
}