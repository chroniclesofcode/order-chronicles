#pragma once
#include <string>
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
            orders.removeOrder(o.id);
        }  else if (o.event_type == 4) {
            /* This occurs when the orderbook indicates that a trade has occured
               for a certain entry in the orderbook. It does not give us the
               entry that caused the match, so we create it ourselves to
               imitate this functionality */
            Order opp(o.price, o.quantity, !o.direction, ID_MAX, 1, o.time);
            orders.addOrder(opp);
        }  else if (o.event_type == 5) {

        }  else if (o.event_type == 6) {

        }  else if (o.event_type == 7) {

        }
        //orders.LOBSTERoutput(1);
    }

    static Order parseMessage(const std::string& s) {
        int j, i = 0;
        incrementNext(s, i, j);
        double otime = stod(s.substr(0, i-1));

        incrementNext(s, i, j);
        int eventtype = stoi(s.substr(j, i));

        incrementNext(s, i, j);
        int oid = stoi(s.substr(j, i));

        incrementNext(s, i, j);
        int osize = stoi(s.substr(j, i));

        incrementNext(s, i, j);
        int oprice = stoi(s.substr(j, i));

        incrementNext(s, i, j);
        int odir = stoi(s.substr(j, i));
        odir = odir == 1 ? 1 : 0;

        //std::cout << "time: " << otime << " etype: " << eventtype << " id: " << oid << " sz: " << osize << " price: " << oprice << " dir " << odir << std::endl;
        return Order(oprice, osize, odir, oid, eventtype, otime);
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
};