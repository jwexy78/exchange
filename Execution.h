#pragma once

#include "Order.h"

struct Execution
{
    Execution(Side _side, quantity_t _quantity, price_t _price,
              const Order& _buyOrder, const Order& _sellOrder)
      : side(_side), quantity(_quantity), price(_price),
        buyOrder(_buyOrder), sellOrder(_sellOrder) {}

    /// The side of the aggressive (taker) order
    Side side;
    /// The quantity that was traded
    quantity_t quantity;
    /// The price at which the trade occurred
    price_t price;
    /// The buy order at the time the trade occured
    Order buyOrder;
    /// The sell order at the time the trade occured
    Order sellOrder;
};