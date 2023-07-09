#pragma once

/*
    Order class
    direction: 0 if sell, 1 if buy
*/
class Order {
public:
    static int order_count;

    int price;
    int quantity;
    int direction;
    int id;

    Order(int price, int quantity, int direction) :  price { price }, quantity{ quantity }, direction{ direction } {
        id = order_count++;
    }
};

