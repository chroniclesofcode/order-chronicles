#pragma once

/*
    Order class
    direction: 0 if sell, 1 if buy
*/
class Order {
public:
    static int order_count;

    int64_t price;
    int quantity;
    int direction;
    int id;
    int event_type;
    double time;

    // corresp variable set only for event type 4 (execute trade) and if
    // the entry refers to an order from the past (before orderbook initialization)
    // Only used for error checking if order matches up properly.
    int corresp; 

    void setCorresp(int c) {
        corresp = c;
    }

    // Used mainly for own testing
    Order(int64_t price, int quantity, int direction) :  price{ price }, quantity{ quantity }, direction{ direction } {
        id = order_count++;
        corresp = -1;
    }

    // Currently used for LOBSTER
    Order(int64_t price, int quantity, int direction, int id, int event_type, double time) : 
    price{ price }, quantity{ quantity }, direction{ direction }, id{ id }, event_type{ event_type }, time{ time } { 
        corresp = -1;
    }
    
};

