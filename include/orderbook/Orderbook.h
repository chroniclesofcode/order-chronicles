#pragma once
#include <set>
#include <map>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <array>
#include <iterator>
#include "Order.h"

/*
    Orderbook using a map (tree) + doubly linked lists.
    Matching engine uses price-time priority.
*/
class Orderbook {
public:
    Orderbook() {
        ask_tot = 0;
        bid_tot = 0;
    }

    void execute_trade(Order bid, Order ask, int price, int quantity) { 
        std::cout << "trade occured for " << quantity << " quantity and $" << ask.price << " giving us " << quantity * ask.price << " return\n";
    }

    void process_limit(Order &o) {
        if (o.direction) {
            int lowest = lowestAsk();
            while (o.price >= lowest && o.quantity > 0) {
                std::list<order_t> &order_q = asks[lowest];
                auto it = order_q.begin();
                while (it != order_q.end() && o.quantity > 0) {
                    auto &ask = *it;
                    if (o.quantity >= ask.quantity) {
                        execute_trade(o, ask, lowest, ask.quantity);
                        o.quantity -= ask.quantity;
                        // ask.quantity = 0;
                        int rmid = ask.id;
                        it++;
                        removeOrder(rmid);
                    } else {
                        execute_trade(o, ask, lowest, o.quantity);
                        ask.quantity -= o.quantity;
                        o.quantity = 0;
                        it++;
                    }
                }
                lowest = lowestAsk();
            }
        } else {
            int highest = highestBid();
            while (o.price <= highest && o.quantity > 0) {
                std::list<order_t> &order_q = bids[highest];
                auto it = order_q.begin();
                while (it != order_q.end() && o.quantity > 0) {
                    auto &bid = *it;
                    if (o.quantity >= bid.quantity) {
                        execute_trade(bid, o, highest, bid.quantity);
                        o.quantity -= bid.quantity;
                        // bid.quantity = 0;
                        int rmid = bid.id;
                        it++;
                        removeOrder(rmid);
                    } else {
                        execute_trade(bid, o, highest, o.quantity);
                        bid.quantity -= o.quantity;
                        o.quantity = 0;
                        it++;
                    }
                }
                highest = highestBid();
            }
        }
    }

    // 1 is bids, 0 is asks
    int addOrder(Order o) {
        process_limit(o);
        if (o.quantity <= 0) {
            return -1;
        }
        if (o.direction) {
            std::list<order_t> &order_q = bids[o.price];
            order_q.push_back(o);
            orders[o.id] = std::prev(order_q.end());

            bid_vol[o.price] += o.quantity;
            bid_tot += o.quantity;
        } else {
            std::list<order_t> &order_q = asks[o.price];
            order_q.push_back(o);
            orders[o.id] = std::prev(order_q.end());

            ask_vol[o.price] += o.quantity;
            ask_tot += o.quantity;
        }
        return o.id;
    }

    void removeOrder(int orderid) {
        if (orderid < 0) return;
        // Get order
        std::list<order_t>::iterator it = orders[orderid];
        Order &o = *it;
        if (o.direction) {
            std::list<order_t> &order_q = bids[o.price];

            order_q.erase(it);
            orders.erase(orderid);
            //info.erase(orderid);

            // Delete key if all orders erased
            if (order_q.empty()) {
                bids.erase(o.price);
            }

            bid_vol[o.price] -= o.quantity;
            bid_tot -= o.quantity;
        } else {
            std::list<order_t> &order_q = asks[o.price];

            order_q.erase(it);
            orders.erase(orderid);
            //info.erase(orderid);

            // Delete key if all orders erased
            if (order_q.empty()) {
                asks.erase(o.price);
            }

            ask_vol[o.price] -= o.quantity;
            ask_tot -= o.quantity;
        }
    }

    int highestBid() {
        if (bids.empty()) return MIN_PRICE;
        return (bids.begin())->first;
    }

    int lowestAsk() {
        if (asks.empty()) return MAX_PRICE;
        return (asks.begin())->first;
    }

    // 1 is bid, 0 is asks
    int totalVolume(bool direction) {
        int ret;
        if (direction) {
            ret = bid_tot;
        } else {
            ret = ask_tot;
        }
        return ret;
    }

    int totalVolume(bool direction, int price) {
        int ret;
        if (direction) {
            ret = bid_vol[price];
        } else {
            ret = ask_vol[price];
        }
        return ret;
    }

private:
    typedef Order order_t;
    std::map<int, std::list<order_t>, std::greater<int>> bids;
    std::map<int, std::list<order_t>> asks;
    std::unordered_map<int, int> ask_vol;
    std::unordered_map<int, int> bid_vol;

    std::unordered_map<int, std::list<order_t>::iterator> orders;
    int ask_tot;
    int bid_tot;

    const int MIN_PRICE = -1;
    const int MAX_PRICE = 9999999;
};