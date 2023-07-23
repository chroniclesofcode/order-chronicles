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

    void printOrderbook() {
        std::cout << "\nBIDS\n";
        for (auto &e : bids) {
            std::cout << '$' << e.first << ": ";
            for (auto i : e.second) {
                std::cout << i.quantity << ' ';
            }
            std::cout << '\n';
        }
        if (bids.empty()) {
            std::cout << "(EMPTY)";
        }
        std::cout << "\nASKS\n";
        if (asks.empty()) {
            std::cout << "(EMPTY)";
        }
        for (auto &e : asks) {
            std::cout << '$' << e.first << ": ";
            for (auto i : e.second) {
                std::cout << i.quantity << ' ';
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }

    void executeTrade(Order bid, Order ask, uint64_t price, int quantity) { 
        //std::cout << "trade occured for " << quantity << " quantity at $" << price << " giving us " << quantity * price << " return\n";
    }

    void processLimit(Order &o) {
        if (o.direction) {
            uint64_t lowest = lowestAsk();
            while (o.price >= lowest && o.quantity > 0) {
                std::list<order_t> &order_q = asks[lowest];
                auto it = order_q.begin();
                while (it != order_q.end() && o.quantity > 0) {
                    order_t &ask = *it;
                    if (o.quantity >= ask.quantity) {
                        executeTrade(o, ask, lowest, ask.quantity);
                        o.quantity -= ask.quantity;
                        // ask.quantity = 0;
                        int rmid = ask.id;
                        it++;
                        removeOrder(rmid);
                    } else {
                        executeTrade(o, ask, lowest, o.quantity);
                        ask.quantity -= o.quantity;
                        o.quantity = 0;
                        it++;
                    }
                }
                // only 'no partial fills' left
                int tmp = lowest;
                lowest = lowestAsk();
                if (tmp == lowest) break;
            }
        } else {
            uint64_t highest = highestBid();
            while (o.price <= highest && o.quantity > 0) {
                std::list<order_t> &order_q = bids[highest];
                auto it = order_q.begin();
                while (it != order_q.end() && o.quantity > 0) {
                    order_t &bid = *it;
                    if (o.quantity >= bid.quantity) {
                        executeTrade(bid, o, highest, bid.quantity);
                        o.quantity -= bid.quantity;
                        // bid.quantity = 0;
                        int rmid = bid.id;
                        it++;
                        removeOrder(rmid);
                    } else {
                        executeTrade(bid, o, highest, o.quantity);
                        bid.quantity -= o.quantity;
                        o.quantity = 0;
                        it++;
                    }
                }
                int tmp = highest;
                // only 'no partial fills' left
                highest = highestBid();
                if (tmp == highest) break;
            }
        }
    }

    // 1 is bids, 0 is asks
    int addOrder(Order &o) {
        processLimit(o);
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

    bool orderIdExists(int orderid) {
        return orders.find(orderid) != orders.end();
    }

    void removeOrder(int orderid) {
        if (!orderIdExists(orderid)) {
            //std::cout << "Error, cannot find order to remove with id " << orderid << '\n';
            //exit(1);
            return;
        }
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
        if (o.direction) {
            std::list<order_t> &order_q = bids[o.price];
            auto it = order_q.begin();
            while (it != order_q.end() && o.quantity > 0) {
                if (it->quantity > o.quantity) {
                    it->quantity -= o.quantity;
                    it++;
                    bid_vol[o.price] -= o.quantity;
                    bid_tot -= o.quantity;
                    o.quantity = 0;
                } else {
                    o.quantity -= it->quantity;

                    auto tmp = it;
                    it++;
                    removeOrder(tmp->id);
                }
            }

            // Delete key if all orders erased
            if (order_q.empty()) {
                bids.erase(o.price);
            }
        } else {
            std::list<order_t> &order_q = asks[o.price];

            auto it = order_q.begin();
            while (it != order_q.end() && o.quantity > 0) {
                if (it->quantity > o.quantity) {
                    it->quantity -= o.quantity;
                    it++;
                    ask_vol[o.price] -= o.quantity;
                    ask_tot -= o.quantity;
                    o.quantity = 0;
                } else {
                    o.quantity -= it->quantity;

                    auto tmp = it;
                    it++;
                    removeOrder(tmp->id);
                }
            }

            // Delete key if all orders erased
            if (order_q.empty()) {
                asks.erase(o.price);
            }
        }
    }

    void modifyOrder(int orderid, int quantity) {
        if (!orderIdExists(orderid)) {
            //std::cout << "Error, cannot find order to modify with id " << orderid << '\n';
            //exit(1);
            return;
        }
        if (quantity == 0) {
            removeOrder(orderid);
            return;
        }
        // Get order
        std::list<order_t>::iterator it = orders[orderid];
        it->quantity = quantity;
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

    void LOBSTERoutput(int level) {
        std::vector<std::array<uint64_t,2>> a, b;
        int ct = 0;
        for (auto &e : asks) {
            if (ct == level) break;
            a.push_back({ e.first, (uint64_t)ask_vol[e.first] });
            ct++;
        }
        ct = 0;
        for (auto &e : bids) {
            if (ct == level) break;
            b.push_back({ e.first, (uint64_t)bid_vol[e.first] });
            ct++;
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
    typedef Order order_t;
    std::map<uint64_t, std::list<order_t>, std::greater<int>> bids;
    std::map<uint64_t, std::list<order_t>> asks;
    std::unordered_map<uint64_t, int> ask_vol;
    std::unordered_map<uint64_t, int> bid_vol;

    std::unordered_map<int, std::list<order_t>::iterator> orders;
    int ask_tot;
    int bid_tot;

    const uint64_t MIN_PRICE = -9999999999;
    const uint64_t MAX_PRICE = 9999999999;
};