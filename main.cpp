#include <iostream>
#include "orderbook/Init.h"

int main(void) {
    std::cout << "Orderbook starting..." << std::endl;
    Init();
    std::cout << "Orderbook finished, exiting..." << std::endl;
    return 0;
}