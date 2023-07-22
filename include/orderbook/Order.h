#pragma once

/*
    Order class
    direction: 0 if sell, 1 if buy
*/
class Order {
public:
    static int order_count;

    uint64_t price;
    int quantity;
    int direction;
    int id;
    int event_type;
    double time;

    // Used mainly for own testing
    Order(uint64_t price, int quantity, int direction) :  price{ price }, quantity{ quantity }, direction{ direction } {
        id = order_count++;
    }

    // Currently used for LOBSTER
    Order(uint64_t price, int quantity, int direction, int id, int event_type, double time) : 
    price{ price }, quantity{ quantity }, direction{ direction }, id{ id }, event_type{ event_type }, time{ time } { }
    
};

