#include <iostream>
#include <string>
#include "orderbook/Init.h"
#include "orderbook/Timer.h"

#define TIMER_ON 0
#define IMPL "LINEAR"

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Incorrect number of arguments for orderbook, terminating..." << '\n';
        return -1;
    }
    std::string book_file("");
    if (argc >= 3) {
        book_file = std::string(argv[2]);
    }
    // If there is a timer, run 10 times for more accurate statistics
    if (TIMER_ON) {
        Timer logger("../stats/results_" IMPL ".md");
        for (int i = 0; i < 100; i++) {
            logger.begin();
            Init(argv[1], book_file);
            logger.stop();
        }
        logger.printStats();
    } else {
        Init(argv[1], book_file);
    }
    return 0;
}