#include <string>
#include <iostream>
#include <fstream>
#include "orderbook/Init.h"
#include "orderbook/LOBSTER/MessageParser.h"

int Order::order_count = 0;

void Init(std::string message_file) {
    Orderbook orders;
    MessageParser parser(orders);

    auto in = std::ifstream(message_file.c_str());
    if (!in.is_open()) {
		throw std::runtime_error("Unable to open file.");
	}
    std::string line;
    while (in.good()) {
        std::getline(in, line);
        if (!line.size()) continue;
        Order o = MessageParser::parseMessage(line);
        parser.receive(o);
    }

    /*
    std::cout << "setting up\n";

    orders.addOrder({ 130, 20, 0 });
    orders.addOrder({ 180, 60, 0 });
    orders.addOrder({ 130, 5, 0 });

    orders.addOrder({ 150, 100, 1 }); // should have 75 left
    orders.addOrder({ 140, 60, 0 });
    orders.addOrder({ 120, 15, 0 });

    orders.addOrder({ 120, 15, 0 }); // shouldn't run

    std::cout << "starting process tests\n";
    */
}