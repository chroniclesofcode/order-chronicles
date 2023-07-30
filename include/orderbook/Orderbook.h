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

typedef struct Entry {
    Order o;
    Entry *next;
} Entry_t;

typedef struct Point {
    Entry_t *head;
    Entry_t *tail;
} Point_t;

static const int MEM_LIMIT = 10000000;
static Point_t prices[MEM_LIMIT]; // All asks and bids (can be same array since we cannot cross spread)

/*
    Orderbook using a map (tree) + doubly linked lists.
    Matching engine uses price-time priority.
*/
class Orderbook {
public:
    Orderbook() {
        ask_tot = 0;
        bid_tot = 0;
        lowest_ask = MAX_PRICE;
        highest_bid = MIN_PRICE;
        memset(prices, 0, MEM_LIMIT * sizeof(Point_t));
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
        return -1;
    }

    bool orderIdExists(int orderid) {
        return true;
    }

    void removeOrder(int orderid) {
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
        return -1;
    }

    int lowestAsk() {
        return -1;
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

    void LOBSTERoutput(int level) {
    }

private:
    typedef Order order_t;

    std::unordered_map<int64_t, int> ask_vol;
    std::unordered_map<int64_t, int> bid_vol;

    std::unordered_map<int, Entry_t*> orders;

    int64_t highest_bid;
    int64_t lowest_ask;
    int ask_tot;
    int bid_tot;

    const int64_t MIN_PRICE = -9999999999;
    const int64_t MAX_PRICE = 9999999999;
};