#include <string>
#include <iostream>
#include <fstream>
#include "orderbook/Init.h"
#include "orderbook/LOBSTER/MessageParser.h"

int Order::order_count = 0;

void Init(std::string message_file, std::string book_file) {
    Orderbook orders;
    MessageParser parser(orders);
    auto in = std::ifstream(message_file.c_str());
    if (!in.is_open()) {
		throw std::runtime_error("Unable to open file.");
	}
    

    if (book_file.length() > 0) {
        auto in2 = std::ifstream(book_file.c_str());
        if (!in2.is_open()) {
            throw std::runtime_error("Unable to open orderbook file.");
        }
        std::string firstline;
        std::getline(in2, firstline);
        if (!firstline.size()) exit(1);
        parser.populate(firstline);
    }

    std::string line;
    // Don't process the first line of messages to align orderbook and 
    // messages properly for LOBSTER
    std::getline(in, line);
    while (in.good()) {
        std::getline(in, line);
        if (!line.size()) continue;
        Order o = MessageParser::parseMessage(line);
        parser.receive(o);
    }
}