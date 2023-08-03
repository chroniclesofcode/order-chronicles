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


/*
    Orderbook using a hashmap attached to custom linked lists.
    Matching engine uses price-time priority.
*/
class Orderbook {
public:
    Orderbook() {
        ask_tot = 0;
        bid_tot = 0;
        lowest_ask = MAX_PRICE;
        highest_bid = MIN_PRICE;
    }

    ~Orderbook() {
    }

    void printOrderbook() {
    }

    void executeTrade(Order bid, Order ask, int64_t price, int quantity, int in_dir) { 
        //std::cout << "TRADE OCCURED\n";
    }

    void processLimit(Order &o) {
        if (o.direction) {
            for (auto it = ask_keys.lower_bound(lowest_ask); it != ask_keys.end() && o.quantity > 0 && o.price >= *it; it++) {
                if (ask_vol[o.price] == 0) continue;
                Point_t &point = prices[*it];
                Entry_t *head = point.head;
                while (head != NULL && o.quantity > 0) {
                    Order &ask = head->o;
                    if (ask.quantity == 0) {
                        head = head->next;
                        continue;
                    }
                    if (o.quantity >= ask.quantity) {
                        executeTrade(o, ask, *it, ask.quantity, o.direction);
                        o.quantity -= ask.quantity;
                        removeOrder(ask.id);
                    } else {
                        executeTrade(o, ask, *it, o.quantity, o.direction);
                        ask.quantity -= o.quantity;
                        ask_vol[*it] -= o.quantity;
                        ask_tot -= o.quantity;
                        o.quantity = 0;
                    }
                    head = head->next;
                }
            }
        } else {
            for (auto it = bid_keys.lower_bound(highest_bid); it != bid_keys.end() && o.quantity > 0 && o.price <= *it; it++) {
                if (bid_vol[o.price] == 0) continue;
                Point_t &point = prices[*it];
                Entry_t *head = point.head;
                while (head != NULL && o.quantity > 0) {
                    Order &bid = head->o;
                    if (bid.quantity == 0) {
                        head = head->next;
                        continue;
                    }
                    if (o.quantity >= bid.quantity) {
                        executeTrade(bid, o, *it, bid.quantity, o.direction);
                        o.quantity -= bid.quantity;
                        removeOrder(bid.id);
                    } else {
                        executeTrade(bid, o, *it, o.quantity, o.direction);
                        bid.quantity -= o.quantity;
                        bid_vol[*it] -= o.quantity;
                        bid_tot -= o.quantity;
                        o.quantity = 0;
                    }
                    head = head->next;
                }
            }
        }
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

        // Creating the entry and putting it into the orders map
        Point_t &point = prices[o.price];
        orders[o.id] = Order(o);
        // Obtain address of entry inside orders to use as pointer
        Entry_t *to_add = &orders[o.id];

        if (point.head == NULL) {
            point.head = to_add;
            point.tail = to_add;
        } else {
            (point.tail)->next = to_add;
            point.tail = to_add;
        }

        if (o.direction) {
            if (bid_vol.find(o.price) == bid_vol.end()) {
            //if (bid_vol[o.price] == 0) {
                bid_keys.insert(o.price);
            }
            bid_vol[o.price] += o.quantity;
            bid_tot += o.quantity;
            highest_bid = std::max(highest_bid, o.price);
        } else {
            if (ask_vol.find(o.price) == ask_vol.end()) {
            //if (ask_vol[o.price] == 0) {
                ask_keys.insert(o.price);
            }
            ask_vol[o.price] += o.quantity;
            ask_tot += o.quantity;
            lowest_ask = std::min(lowest_ask, o.price);
        }
        return o.id;
    }

    bool orderIdExists(int orderid) {
        return orders.find(orderid) != orders.end();
    }

    // Limitation of current implementation: 
    // we cannot reuse IDs + cannot remove them from
    // the orders map - since our linked list is dependent
    // on the entries to function properly. May fix later,
    // however this restriction should make our implementation
    // more performant. Our solution sets quantity to 0.
    void removeOrder(int orderid) {
        if (!orderIdExists(orderid)) {
            return;
        }

        Entry_t &e = orders[orderid];
        Order &o = e.o;
        //orders.erase(orderid);
        if (o.id != orderid) {
            std::cout << "broken\n";
            exit(1);
        }
        if (o.direction) {
            bid_vol[o.price] -= o.quantity;
            bid_tot -= o.quantity;
        } else {
            ask_vol[o.price] -= o.quantity;
            ask_tot -= o.quantity;
        }
        o.quantity = 0;
        calibrateSpread(o);
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
                if (o.direction) {
                    bid_vol[o.price] -= o.quantity;
                    bid_tot -= o.quantity;
                } else {
                    ask_vol[o.price] -= o.quantity;
                    ask_tot -= o.quantity;
                }
                o.quantity = 0;
            } else {
                o.quantity -= curr.quantity;
                removeOrder(curr.id);
            }
            front = front->next;
        }
        calibrateSpread(o);
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
        int ret = 0;
        if (direction) {
            if (bid_vol.find(price) != bid_vol.end()) {
                ret = bid_vol[price];
            }
        } else {
            if (ask_vol.find(price) != ask_vol.end()) {
                ret = ask_vol[price];
            }
        }
        return ret;
    }

    void LOBSTERoutput(int level) {
        std::vector<std::array<int64_t,2>> a, b;
        int ct = 0;
        for (auto &v : ask_keys) {
            if (ct == level) break;
            if (ask_vol[v] > 0) {
                a.push_back({ v, (int64_t)ask_vol[v] });
                ct++;
            }
        }
        ct = 0;
        for (auto &v : bid_keys) {
            if (ct == level) break;
            if (bid_vol[v] > 0) {
                b.push_back({ v, (int64_t)bid_vol[v] });
                ct++;
            }
        }
        for (int i = 0; i < level; i++) {
            if (a.size() > i) {
                std::cout << a[i][0] << ',' << a[i][1] << ',';
            } else {
                std::cout << MAX_PRICE << ',' << 0 << ',';
            }
            if (b.size() > i) {
                std::cout << b[i][0] << ',' << b[i][1];
            } else {
                std::cout << MIN_PRICE << ',' << 0;
            }
            std::cout << (i == level - 1 ? '\n' : ',');
        }
    }

private:
    /* ---- HELPER FUNCTIONS ---- */

    void calibrateSpread(Order &o) {
        if (o.direction) {
            // Find the next highest bid
            if (bid_tot == 0) {
                highest_bid = MIN_PRICE;
            } else if (bid_vol[o.price] == 0) {
                for (auto it = bid_keys.lower_bound(o.price); it != bid_keys.end(); it++) {
                    if (bid_vol.find(*it) != bid_vol.end() && bid_vol[*it] > 0) {
                        highest_bid = *it;
                        break;
                    }
                }
                /*
                for (int64_t i = o.price; i >= 0; i -= 50) {
                    if (bid_vol.find(i) != bid_vol.end() && bid_vol[i] > 0) {
                        highest_bid = i;
                        break;
                    }
                }
                */
                prices[o.price].head = NULL;
                prices[o.price].tail = NULL;
            }
        } else {
            // Find the next lowest ask
            if (ask_tot == 0) {
                lowest_ask = MAX_PRICE;
            } else if (ask_vol[o.price] == 0) {
                for (auto it = ask_keys.lower_bound(o.price); it != ask_keys.end(); it++) {
                    if (ask_vol.find(*it) != ask_vol.end() && ask_vol[*it] > 0) {
                        lowest_ask = *it;
                        break;
                    }
                }
                /*
                for (int64_t i = o.price; i <= MAX_PRICE; i += 50) {
                    if (ask_vol.find(i) != ask_vol.end() && ask_vol[i] > 0) {
                        lowest_ask = i;
                        break;
                    }
                }
                */
                prices[o.price].head = NULL;
                prices[o.price].tail = NULL;
            }
        }
    }

    /* ---- END HELPER FUNCTIONS ---- */

    typedef Order order_t;

    std::unordered_map<int64_t, int> ask_vol;
    std::unordered_map<int64_t, int> bid_vol;

    // map to Entry_t, the address of each entry_t is used as part of
    // the linked list
    std::unordered_map<int, Entry_t> orders;

    // Array of all bids/asks. Can be in one array because we can't cross the spread
    std::unordered_map<int64_t, Point_t> prices;

    // Many tradeoffs happening here. Since ordered_map implementation allows for easy
    // LOBSTER level processing, it is unrealistic to ask that for 'array' implementation
    // How are we meant to traverse price levels? So as a sacrifice, we will add all
    // keys to a vector during the add phase, for a slight but insignificant slowdown.
    std::set<int64_t, std::greater<int64_t>> bid_keys;
    std::set<int64_t> ask_keys;

    int64_t highest_bid;
    int64_t lowest_ask;
    int ask_tot;
    int bid_tot;

    const int64_t MIN_PRICE = -9999999999;
    const int64_t MAX_PRICE = 9999999999;
};