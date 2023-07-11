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

    int addOrder(Order o) {
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

    void cancelOrder(int orderid) {
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
        if (bids.empty()) return -1;
        return (bids.begin())->first;
    }

    int lowestAsk() {
        if (asks.empty()) return -1;
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
};