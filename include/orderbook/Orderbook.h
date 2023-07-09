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
    Orderbook using a map (tree) + doubly linked lists
*/
class Orderbook {
public:
    Orderbook() {
        ask_tot = 0;
        bid_tot = 0;
    }

    void addOrder(Order o) {
        if (o.direction) {
            std::list<order_t> &order_q = bids[o.price];
            order_q.push_back({ o.quantity, o.id });
            orders[o.id] = std::prev(order_q.end());
            dirs[o.id] = 1;

            bid_vol[o.price] += o.quantity;
            bid_tot += o.quantity;
        } else {
            std::list<order_t> &order_q = asks[o.price];
            order_q.push_back({ o.quantity, o.id });
            orders[o.id] = std::prev(order_q.end());
            dirs[o.id] = 1;

            ask_vol[o.price] += o.quantity;
            ask_tot += o.quantity;
        }
    }

    void cancelOrder(int orderid) {
        // Get order
        std::list<order_t>::iterator it = orders[orderid];
        int o_price = (*it)[0];
        int o_quantity = (*it)[1];
        int o_direction = dirs[orderid];
        if (o_direction) {
            std::list<order_t> &order_q = bids[o_price];

            order_q.erase(it);
            orders.erase(orderid);
            //dirs.erase(orderid);

            bid_vol[o_price] -= o_quantity;
            bid_tot -= o_quantity;
        } else {
            std::list<order_t> &order_q = asks[o_price];

            order_q.erase(it);
            orders.erase(orderid);
            //dirs.erase(orderid);

            ask_vol[o_price] -= o_quantity;
            ask_tot -= o_quantity;
        }
    }

    int highestBid() {
        if (bids.empty()) return -1;
        return (*bids.begin()).first;
    }

    int lowestAsk() {
        if (asks.empty()) return -1;
        return (*asks.begin()).first;
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
    typedef std::array<int, 2> order_t;
    std::map<int, std::list<order_t>, std::greater<int>> bids;
    std::map<int, std::list<order_t>> asks;
    std::unordered_map<int, int> ask_vol;
    std::unordered_map<int, int> bid_vol;

    std::unordered_map<int, std::list<order_t>::iterator> orders;
    std::unordered_map<int, int> dirs;
    int ask_tot;
    int bid_tot;
};