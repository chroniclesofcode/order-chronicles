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

    Point() {}
    Point(int64_t p) : price{ p } { orders.reserve(50000); }
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
        std::cout << "\nBIDS\n";
        for (auto &e : bids) {
            std::cout << '$' << e.price << ": ";
            for (auto o : e.orders) {
                std::cout << "(" << o.quantity << "," << o.id << ") ";
            }
            std::cout << '\n';
        }
        if (bids.empty()) {
            std::cout << "(EMPTY)";
        }
        std::cout << "\nASKS\n";
        for (auto &e : asks) {
            std::cout << '$' << e.price << ": ";
            for (auto o : e.orders) {
                std::cout << "(" << o.quantity << "," << o.id << ") ";
            }
            std::cout << '\n';
        }
        if (asks.empty()) {
            std::cout << "(EMPTY)" << " bids: " << bids.size() << " asks: " << asks.size() << '\n';
        }
        std::cout << '\n';
    }

    void executeTrade(Order bid, Order ask, int64_t price, int quantity, int in_dir) { 
        //std::cout << "TRADE OCCURED\n";
    }

    void processLimit(Order &o) {
        if (o.direction) {
            auto it = asks.begin();
            while(it != asks.end() && o.quantity > 0 && o.price >= it->price) {
                if (ask_vol[o.price] == 0) continue;
                std::vector<Order> &curr = it->orders;
                int psz = curr.size();
                int cprice = it->price;
                for (int i = 0; i < psz && o.quantity > 0; i++)  {
                    if (curr[i].quantity == 0) continue;
                    if (o.quantity >= curr[i].quantity) {
                        executeTrade(o, curr[i], cprice, curr[i].quantity, o.direction);
                        o.quantity -= curr[i].quantity;
                        bool flag = false;
                        if (ask_vol[cprice] - curr[i].quantity == 0) {
                            it++;
                            flag = true;
                        }
                        removeOrder(curr[i].id);
                        if (flag) {
                            break;
                        } else {
                            it++;
                        }
                    } else {
                        executeTrade(curr[i], o, cprice, o.quantity, o.direction);
                        curr[i].quantity -= o.quantity;
                        ask_vol[cprice] -= o.quantity;
                        ask_tot -= o.quantity;
                        o.quantity = 0;
                        it++;
                    }
                }
            }
        } else {
            auto it = bids.begin();
            while(it != bids.end() && o.quantity > 0 && o.price <= it->price) {
                if (bid_vol[o.price] == 0) continue;
                std::vector<Order> &curr = it->orders;
                int psz = curr.size();
                int cprice = it->price;
                for (int i = 0; i < psz && o.quantity > 0; i++)  {
                    if (curr[i].quantity == 0) continue;
                    if (o.quantity >= curr[i].quantity) {
                        executeTrade(curr[i], o, cprice, curr[i].quantity, o.direction);
                        o.quantity -= curr[i].quantity;
                        bool flag = false;
                        if (bid_vol[cprice] - curr[i].quantity == 0) {
                            it++;
                            flag = true;
                        }
                        removeOrder(curr[i].id);
                        if (flag) {
                            break;
                        } else {
                            it++;
                        }
                    } else {
                        executeTrade(curr[i], o, cprice, o.quantity, o.direction);
                        curr[i].quantity -= o.quantity;
                        bid_vol[cprice] -= o.quantity;
                        bid_tot -= o.quantity;
                        o.quantity = 0;
                        it++;
                    }
                }
            }
        }
    }

    // 1 is bids, 0 is asks
    int addOrder(Order &o) { 
        if (o.corresp != -1) {
            Order *curr = orders[o.corresp];
            if (o.quantity >= curr->quantity) {
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
        if (o.direction) {
            // Loop until value lower than price or equal
            auto it = bids.begin();
            while (it != bids.end() && it->price > o.price) {
                it++;
            }
            // Price equal, insert value
            if (it != bids.end() && it->price == o.price) {
                std::vector<Order> &curr = it->orders;
                curr.push_back(o);
                orders[o.id] = &(curr.back());
            } else {
                auto added = bids.insert(it, point_t(o.price));
                added->orders.push_back(o);
                orders[o.id] = &(added->orders.back());
                prices[o.price] = added;
            }
            bid_vol[o.price] += o.quantity;
            bid_tot += o.quantity;
        } else {
            // Loop until value higher than price or equal
            auto it = asks.begin();
            while (it != asks.end() && it->price < o.price) {
                it++;
            }
            // Price equal, insert value
            if (it != asks.end() && it->price == o.price) {
                std::vector<Order> &curr = it->orders;
                curr.push_back(o);
                orders[o.id] = &(curr.back());
            } else {
                auto added = asks.insert(it, point_t(o.price));
                added->orders.push_back(o);
                orders[o.id] = &(added->orders.back());
                prices[o.price] = added;
            }
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
            return;
        }
        Order *o = orders[orderid];
        if (orderid != o->id) {
            std::cout << "wrong orderid - correct: " << orderid << " wrong: " << o->id << " " << o->price <<'\n';
            std::cout << "BROKEN\n";
            exit(1);
        }
        if (o->quantity == 0) {
            return;
        }
        int remid = o->id;
        int64_t remprice = o->price;
        if (o->direction) {
            bid_vol[remprice] -= o->quantity;
            bid_tot -= o->quantity;
            o->quantity = 0;
            if (bid_vol[remprice] == 0 && prices.find(remprice) != prices.end()) {
                bids.erase(prices[remprice]);
                prices.erase(remprice);
            }
        } else {
            ask_vol[remprice] -= o->quantity;
            ask_tot -= o->quantity;
            o->quantity = 0;
            if (ask_vol[remprice] == 0 && prices.find(remprice) != prices.end()) {
                asks.erase(prices[remprice]);
                prices.erase(remprice);
            }
        }
        orders.erase(remid);
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
            point_t &order_q = *prices[o.price];
            std::vector<Order> &v = order_q.orders;
            for (int i = 0; i < v.size() && o.quantity > 0; i++) {
                if (v[i].quantity > o.quantity) {
                    v[i].quantity -= o.quantity;
                    bid_vol[o.price] -= o.quantity;
                    bid_tot -= o.quantity;
                    o.quantity = 0;
                } else {
                    o.quantity -= v[i].quantity;
                    removeOrder(v[i].id);
                    if (bid_vol[o.price] == 0) {
                        break;
                    }
                }
            }
            if (bid_vol[o.price] == 0 && prices.find(o.price) != prices.end()) {
                bids.erase(prices[o.price]);
                prices.erase(o.price);
            }
        } else {
            point_t &order_q = *prices[o.price];
            std::vector<Order> &v = order_q.orders;
            for (int i = 0; i < v.size() && o.quantity > 0; i++) {
                if (v[i].quantity > o.quantity) {
                    v[i].quantity -= o.quantity;
                    ask_vol[o.price] -= o.quantity;
                    ask_tot -= o.quantity;
                    o.quantity = 0;
                } else {
                    o.quantity -= v[i].quantity;
                    removeOrder(v[i].id);
                    if (ask_vol[o.price] == 0) {
                        break;
                    }
                }
            }
            if (ask_vol[o.price] == 0 && prices.find(o.price) != prices.end()) {
                asks.erase(prices[o.price]);
                prices.erase(o.price);
            }
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
        Order *o = orders[orderid];
        if (quantity >= o->quantity) {
            removeOrder(orderid);
            return;
        }
        if (o->direction) {
            bid_tot -= quantity;
            bid_vol[o->price] -= quantity;
        } else {
            ask_tot -= quantity;
            ask_vol[o->price] -= quantity;
        }
        o->quantity -= quantity;
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
        //printOrderbook();
        std::vector<std::array<int64_t,2>> a, b;
        int ct = 0;
        // price points in asks
        for (auto &pp : asks) {
            if (ct == level) break;
            a.push_back({ pp.price, (int64_t)ask_vol[pp.price] });
            ct++;
        }
        ct = 0;
        for (auto &pp : bids) {
            if (ct == level) break;
            b.push_back({ pp.price, (int64_t)bid_vol[pp.price] });
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

    std::unordered_map<int64_t, int> ask_vol;
    std::unordered_map<int64_t, int> bid_vol;

    std::unordered_map<int, std::list<point_t>::iterator> prices;
  
    std::unordered_map<int, Order*> orders;

    std::list<point_t> bids; // sorted high->low
    std::list<point_t> asks; // sorted low->high

    int ask_tot;
    int bid_tot;

    const int64_t MIN_PRICE = -9999999999;
    const int64_t MAX_PRICE = 9999999999;
};