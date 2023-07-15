#include <string>
#include <fstream>
#include <cstdlib>
#include "orderbook/Init.h"
#include "orderbook/LOBSTER/MessageParser.h"

int Order::order_count = 0;

void Init(std::string message_file) {
    Orderbook orders;

    FILE *msgs;
    try {
        msgs = fopen(message_file.c_str(), 'r');
        if (msg == NULL) throw;
    } catch (...) {
        std::cout << "Error opening file\n";
        exit(1);
    }

    std::cout << "setting up\n";

    orders.addOrder({ 130, 20, 0 });
    orders.addOrder({ 180, 60, 0 });
    orders.addOrder({ 130, 5, 0 });

    orders.addOrder({ 150, 100, 1 }); // should have 75 left
    orders.addOrder({ 140, 60, 0 });
    orders.addOrder({ 120, 15, 0 });

    orders.addOrder({ 120, 15, 0 }); // shouldn't run

    std::cout << "starting process tests\n";
}