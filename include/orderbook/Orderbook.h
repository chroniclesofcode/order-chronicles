#pragma once
#include <set>
#include <map>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <array>
#include <vector>
#include <iterator>
#include <limits>
#include <cstdint>
#include "Order.h"

/*
    Orderbook - implementation with LINEAR data structures
    Matching engine uses price-time priority.
*/

typedef struct Point {
    int64_t price;
    std::vector<Order> orders;
} point_t;

class Orderbook {
public:
    Orderbook() {
        ask_tot = 0;
        bid_tot = 0;
    }

    ~Orderbook() {
    }

    void printOrderbook() {
    }

    void executeTrade(Order bid, Order ask, int64_t price, int quantity, int in_dir) { 
        //std::cout << "TRADE OCCURED\n";
    }

    void processLimit(Order &o) {

    }

    // 1 is bids, 0 is asks
    int addOrder(Order &o) { 
        if (o.corresp != -1) {
            point_t &rem = *(prices[o.price]);
            Order &curr = rem.orders[pidx[o.corresp]];
            if (o.quantity >= curr.quantity) {
                removeOrder(o.corresp);
            } else {
                modifyOrder(o.corresp, o.quantity);
            }
            return -1;
        }
        return o.id;
    }

    bool orderIdExists(int orderid) {
        return pidx.find(orderid) != pidx.end();
    }

    void removeOrder(int orderid) {
        if (!orderIdExists(orderid)) {
            return;
        }
    }

    /* This function is very unsafe - try not to use it unless needed. It will
       take in an order, and forcibly erase price * quantity value of orders 
       from the front of the price-priority queue. Currently, useful for when
       initializing a LOBSTER orderbook, and we need to cancel an order from
       previous time periods, so we don't actually have the order + orderid 
       in the system. This will force-cancel some orders to ensure the orderbook
       remains synced on our side and the LOBSTER side. Also, if all orders are
       removed, it will stop there - it should not move onto the next price
       point. */
    void forceRemoveOrder(Order &o) {

    }

    void modifyOrder(int orderid, int quantity) {
 
    }

    int highestBid() {
        if (bids.empty()) return MIN_PRICE;
        return (*bids.begin()).price;
    }

    int lowestAsk() {
        if (asks.empty()) return MAX_PRICE;
        return (*asks.begin()).price;
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

    int totalVolume(bool direction, int64_t price) {
        int ret = 0;
        if (direction) {
            ret = bid_vol[price];
        } else {
            ret = ask_vol[price];
        }
        return ret;
    }

    void LOBSTERoutput(int level) {
    }

private:
    typedef Order order_t;

    std::unordered_map<int64_t, int> ask_vol;
    std::unordered_map<int64_t, int> bid_vol;

    std::unordered_map<int, std::list<point_t>::iterator> prices;
    // pidx: order_id -> index at price point
    std::unordered_map<int, int> pidx;
    // idp: order_id -> to its price
    std::unordered_map<int, int> idp;

    std::list<order_t> bids; // sorted high->low
    std::list<order_t> asks; // sorted low->high

    int ask_tot;
    int bid_tot;

    const int64_t MIN_PRICE = -9999999999;
    const int64_t MAX_PRICE = 9999999999;
};