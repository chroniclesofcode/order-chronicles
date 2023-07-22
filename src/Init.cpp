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
    
    bool HAS_BOOK = false;

    if (book_file.length() > 0) {
        auto in2 = std::ifstream(book_file.c_str());
        if (!in2.is_open()) {
            throw std::runtime_error("Unable to open orderbook file.");
        }
        std::string firstline;
        std::getline(in2, firstline);
        if (!firstline.size()) exit(1);
        parser.populate(firstline);
        HAS_BOOK = true;
    }

    std::string line;
    while (in.good()) {
        std::getline(in, line);
        if (!line.size()) continue;
        // If populated orderbook with entries, we skip the first line to align
        // our book and LOBSTER book properly
        if (HAS_BOOK) {
            HAS_BOOK = false;
            continue;
        }
        Order o = MessageParser::parseMessage(line);
        parser.receive(o);
    }
}