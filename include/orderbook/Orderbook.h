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

    Entry() {
        next = NULL;
    }

    Entry(Order add) : o{ add } {
        next = NULL;
    }
} Entry_t;

typedef struct Point {
    Entry_t *head = NULL;
    Entry_t *tail = NULL;
} Point_t;

static const int MEM_LIMIT = 10000000;

/*
    Orderbook using an array attached to custom linked lists.
    Matching engine uses price-time priority.
*/
class Orderbook {
public:
    Orderbook() {
        ask_tot = 0;
        bid_tot = 0;
        lowest_ask = MAX_PRICE;
        highest_bid = MIN_PRICE;
        prices = new Point_t[MEM_LIMIT];
    }

    ~Orderbook() {
        delete[] prices;
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
            Entry_t &e = orders[o.corresp];
            Order &curr = e.o;

            if (o.quantity >= curr.quantity) {
                removeOrder(o.corresp);
            } else {
                modifyOrder(o.corresp, o.quantity);
            }
            return -1;
        }
        processLimit(o);
        if (o.quantity <= 0) {
            return -1;
        }
        /*

        // Creating the entry and putting it into the orders map
        Point_t &point = prices[o.price];
        Entry_t new_entry(o);
        orders[o.id] = new_entry;

        // Obtain address of entry inside orders to use as pointer
        Entry_t *to_add = &orders[o.id];

        if (point.head == NULL || bid_vol[o.price] == 0) {
            point.head = to_add;
            point.tail = to_add;
        } else {
            (point.tail)->next = to_add;
            point.tail = to_add;
        }

        if (o.direction) {
            bid_vol[o.price] += o.quantity;
            bid_tot += o.quantity;
        } else {
            ask_vol[o.price] += o.quantity;
            ask_tot += o.quantity;
        }
        */
        return o.id;
    }

    bool orderIdExists(int orderid) {
        return orders.find(orderid) != orders.end();
    }

    void removeOrder(int orderid) {
        if (!orderIdExists(orderid)) {
            return;
        }

        Entry_t &e = orders[orderid];
        Order &o = e.o;

        o.quantity = 0;
        orders.erase(orderid);

        if (o.direction) {
            bid_vol[o.price] -= o.quantity;
            bid_tot -= o.quantity;
        } else {
            ask_vol[o.price] -= o.quantity;
            ask_tot -= o.quantity;
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
        Point_t &point = prices[o.price];
        Entry_t *front = point.head;

        while (front != NULL && o.quantity > 0) {
            Order &curr = front->o;
            if (curr.quantity == 0) {
            } else if (curr.quantity > o.quantity) {
                curr.quantity  -= o.quantity;
                bid_vol[o.price] -= o.quantity;
                bid_tot -= o.quantity;
                o.quantity = 0;
            } else {
                o.quantity -= curr.quantity;
                removeOrder(curr.id);
            }
            front = front->next;
        }
    }

    void modifyOrder(int orderid, int quantity) {
        if (!orderIdExists(orderid)) {
            return;
        }
        if (quantity == 0) {
            removeOrder(orderid);
            return;
        }
        Entry_t &e = orders[orderid];
        Order &o = e.o;

        if (quantity >= o.quantity) {
            removeOrder(orderid);
            return;
        }
        if (o.direction) {
            bid_vol[o.price] -= quantity;
            bid_tot -= quantity;
        } else {
            ask_vol[o.price] -= quantity;
            ask_tot -= quantity;
        }
        o.quantity -= quantity;
    }

    int highestBid() {
        return highest_bid;
    }

    int lowestAsk() {
        return lowest_ask;
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

    // map to Entry_t, the address of each entry_t is used as part of
    // the linked list
    std::unordered_map<int, Entry_t> orders;

    // Array of all bids/asks. Can be in one array because we can't cross the spread
    Point_t *prices;

    int64_t highest_bid;
    int64_t lowest_ask;
    int ask_tot;
    int bid_tot;

    const int64_t MIN_PRICE = -9999999999;
    const int64_t MAX_PRICE = 9999999999;
};