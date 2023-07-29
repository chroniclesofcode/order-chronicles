#pragma once
#include <string>
#include <cstdint>
#include "../Order.h"
#include "../Orderbook.h"

class MessageParser {
public: 
    MessageParser(Orderbook &orders) : orders{ orders } {}

    void receive(Order &o) {
        if (o.event_type == 1) {
            orders.addOrder(o);
        } else if (o.event_type == 2) {
            orders.modifyOrder(o.id, o.quantity);
        }  else if (o.event_type == 3) {
            if (orders.orderIdExists(o.id)) {
                orders.removeOrder(o.id);
            } else {
                orders.forceRemoveOrder(o);
            }
        }  else if (o.event_type == 4) {
            /* This occurs when the orderbook indicates that a trade has occured
               for a certain entry in the orderbook. It does not give us the
               entry that caused the match, so we create it ourselves to
               imitate this functionality */
            Order opp(o.price, o.quantity, !o.direction, ID_MAX, 1, o.time);
            if (orders.orderIdExists(o.id)) {
                opp.setCorresp(o.id);
            }
            orders.addOrder(opp);
        }  else if (o.event_type == 5) {
            // Unrelated
        }  else if (o.event_type == 6) {
            // Unrelated
        }  else if (o.event_type == 7) {
            // Unrelated
        }
        orders.LOBSTERoutput(LOB_NUM);
    }

    static Order parseMessage(const std::string& s) {
        int j = -1, i = 0;
        incrementNext(s, i, j);
        double otime = stod(s.substr(0, i-1));

        incrementNext(s, i, j);
        int eventtype = stoi(s.substr(j, i-j));

        incrementNext(s, i, j);
        int oid = stoi(s.substr(j, i-j));

        incrementNext(s, i, j);
        int osize = stoi(s.substr(j, i-j));

        incrementNext(s, i, j);
        int64_t oprice = stoll(s.substr(j, i-j));

        incrementNext(s, i, j);
        int odir = stoi(s.substr(j, i-j));
        odir = odir == 1 ? 1 : 0;

        //std::cout << "time: " << otime << " etype: " << eventtype << " id: " << oid << " sz: " << osize << " price: " << oprice << " dir " << odir << std::endl;
        return Order(oprice, osize, odir, oid, eventtype, otime);
    }

    // Populates orderbook with the entries from the LOBSTER orderbook file
    // Not 100% accurate reflection of the entire orderbook, but the
    // first 50 price levels should generally enough for a couple hundred
    // trades.
    // Currently just using increasing order-ids -> not optimal but LOBSTER
    // data will be very hard to find order-ids below 2 digits
    void populate(const std::string &s) {
        int start_id = 0;
        int i = -1, j = -1;
        int ct = 0;
        int64_t price = 0;
        int quantity = 0;
        while (i < (int)s.size()) {
            incrementNext(s, i, j);
            if (ct % 4 == 0) {
                price = stoll(s.substr(j, i-j));
            } else if (ct % 4 == 1) {
                quantity = stoi(s.substr(j, i-j));
                Order n = Order(price, quantity, 0, start_id, 1, 0.0);
                if (price > 0)
                    orders.addOrder(n);
                start_id++;
            } else if (ct % 4 == 2) {
                price = stoll(s.substr(j, i-j));
            } else if (ct % 4 == 3) {
                quantity = stoi(s.substr(j, i-j));
                Order n = Order(price, quantity, 1, start_id, 1, 0.0);
                if (price > 0) 
                    orders.addOrder(n);
                start_id++;
            }
            ct++;
        }
        orders.LOBSTERoutput(LOB_NUM);
    }
private:
    static void incrementNext(const std::string &s, int &i, int &j) {
        j = i+1;
        i = j;
        while (i < s.size() && s[i] != ',') {
            i++;
        }
    }

    Orderbook &orders;
    int ID_MAX = std::numeric_limits<int>::max();
    const int LOB_NUM = 50;
};