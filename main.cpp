#include <iostream>
#include <string>
#include "orderbook/Init.h"

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Incorrect number of arguments for orderbook, terminating..." << '\n';
        return -1;
    }
    std::cout << "Orderbook starting..." << std::endl;
    std::string book_file("");
    if (argc >= 3) {
        book_file = std::string(argv[2]);
    }
    Init(argv[1], book_file);
    std::cout << "Orderbook finished, exiting..." << std::endl;
    return 0;
}