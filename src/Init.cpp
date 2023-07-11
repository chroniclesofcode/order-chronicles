#include "orderbook/Init.h"

int Order::order_count = 0;

void Init() {
    Orderbook orders;
    orders.addOrder({ 150, 100, 1 });
    orders.addOrder({ 120, 60, 1 });
    orders.addOrder({ 130, 20, 0 });
    orders.addOrder({ 180, 60, 0 });
    orders.addOrder({ 130, 5, 0 });
    std::cout << orders.highestBid() << std::endl;
    std::cout << orders.lowestAsk() << std::endl;
    std::cout << orders.totalVolume(1) << std::endl;
    std::cout << orders.totalVolume(0) << std::endl;
    std::cout << orders.totalVolume(0, 130) << std::endl;
    std::cout << "starting order cancellation tests\n";
    int c1 = orders.addOrder({ 100, 21, 0 });
    std::cout << orders.lowestAsk() << std::endl;
    std::cout << orders.totalVolume(0) << std::endl;
    int c2 = orders.addOrder({ 110, 27, 0 });
    std::cout << orders.lowestAsk() << std::endl;
    std::cout << orders.totalVolume(0) << std::endl;
    orders.cancelOrder(c1);
    std::cout << orders.lowestAsk() << std::endl;
    std::cout << orders.totalVolume(0) << std::endl;
    orders.cancelOrder(c2);
    std::cout << orders.lowestAsk() << std::endl;
    std::cout << orders.totalVolume(0) << std::endl;
}