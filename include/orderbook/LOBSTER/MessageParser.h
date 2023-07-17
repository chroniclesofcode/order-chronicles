#pragma once
#include <string>
#include "../Order.h"

class MessageParser {
public:

    static void incrementNext(const std::string &s, int &i, int &j) {
        j = i+1;
        i = j;
        while (i < s.size() && s[i] != ',') {
            i++;
        }
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

        std::cout << "time: " << otime << " etype: " << eventtype << " id: " << oid << " sz: " << osize << " price: " << oprice << " dir " << odir << std::endl;
        return Order(0, 0, 0);
    }
};