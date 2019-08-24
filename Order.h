#pragma once

using price_t = unsigned int;
using quantity_t = unsigned int;
enum class Side {Buy, Sell};

using order_id_t = unsigned int;
static order_id_t gid;

struct Order
{
    Order(Side _side, quantity_t _quantity, price_t _price)
      : side(_side), quantity(_quantity), price(_price), id(gid++) {}
    Side side;
    quantity_t quantity;
    price_t price;
    order_id_t id;

    /// Why C++ decided to make us define this is dumb af
    bool operator==(const Order& other) const
    {
        return side == other.side &&
               quantity == other.quantity &&
               price == other.price &&
               id == other.id;
    };
};