#pragma once

#include <map>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

#include "Order.h"
#include "Execution.h"

class Book
{
  public:
    /// Add an order to the book
    /// @return a list of executions that the order generated
    std::vector<Execution> addOrder(Order order);

    /// Cancel a submitted order
    /// @return if the order was succesfully cancelled
    bool cancelOrder(order_id_t orderid);

    /// Return if the book has a buy order, at any price
    bool hasBid() const;
    /// Return if the book has a sell order, at any price
    bool hasOffer() const;

    /// Get the price of the best bid (buy order) on the book
    price_t getBestBid() const;
    /// Get the price of the best offer (sell order) on the book
    price_t getBestOffer() const;

    /// Get the side at which a current price currently is
    /// in the book. Note that for a price between the bid/ask spread,
    /// the result is indeterminate
    Side getSideForLevel(price_t price) const;
    /// Get the quantity currently on the book for a price level
    quantity_t getQuantityForLevel(price_t price) const;

    /// Old print method for viewing a book. This is generally deprecated
    void print(price_t minPrice = 1, price_t maxPrice = 20,
               quantity_t maxQuantity = 50) const;

  private:
    Execution trade(Order& order, Order& against);
    void addOrderToBook(Order order);

    std::map<price_t,std::vector<Order>,std::greater<price_t>> _buyOrders;
    std::map<price_t,std::vector<Order>> _sellOrders;
};

std::vector<Execution> Book::addOrder(Order order)
{
    assert(order.price != 0);
    assert(order.quantity != 0);
    std::vector<Execution> executions;
    if (order.side == Side::Buy) {
        auto sellItr = _sellOrders.begin();
        while (sellItr != _sellOrders.end()) {
            auto& topOrder = sellItr->second.front();
            if (topOrder.price > order.price) {
                break;
            }
            executions.emplace_back(trade(order, topOrder));
            if (topOrder.quantity == 0) {
                sellItr->second.erase(sellItr->second.begin());
                if (sellItr->second.size() == 0) {
                    sellItr = _sellOrders.erase(sellItr);
                }
            }
            if (order.quantity == 0) {
                break;
            }
        }
    } else {
        auto buyItr = _buyOrders.begin();
        while (buyItr != _buyOrders.end()) {
            auto& topOrder = buyItr->second.front();
            if (topOrder.price < order.price) {
                break;
            }
            executions.emplace_back(trade(order, topOrder));
            if (topOrder.quantity == 0) {
                buyItr->second.erase(buyItr->second.begin());
                if (buyItr->second.size() == 0) {
                    buyItr = _buyOrders.erase(buyItr);
                }
            }
            if (order.quantity == 0) {
                break;
            }
        }
    }
    if (order.quantity != 0) {
        addOrderToBook(order);
    }
    return executions;
}

bool Book::cancelOrder(order_id_t orderid)
{
    {
        auto mapItr = _buyOrders.begin();
        while (mapItr != _buyOrders.end()) {
            auto orderItr = mapItr->second.begin();
            while (orderItr != mapItr->second.end()) {
                if (orderItr->id == orderid) {
                    mapItr->second.erase(orderItr);
                    return true;
                }
                ++orderItr;
            }
            ++mapItr;
        }
    }
    {
        auto mapItr = _sellOrders.begin();
        while (mapItr != _sellOrders.end()) {
            auto orderItr = mapItr->second.begin();
            while (orderItr != mapItr->second.end()) {
                if (orderItr->id == orderid) {
                    mapItr->second.erase(orderItr);
                    return true;
                }
                ++orderItr;
            }
            ++mapItr;
        }
    }
    return false;
}

Execution Book::trade(Order& order, Order& against)
{
    // Default with garbage, will set later
    Order buyOrder(order);
    Order sellOrder(order);

    if (order.side == Side::Buy) {
        assert(order.price >= against.price);
        buyOrder = order;
        sellOrder = against;
    } else {
        assert(order.price <= against.price);
        buyOrder = against;
        sellOrder = order;
    }
    Side executionSide = order.side;
    price_t executionPrice = (order.price + against.price) / 2;
    quantity_t executionQuantity = std::min(order.quantity, against.quantity);
    order.quantity -= executionQuantity;
    against.quantity -= executionQuantity;
    return Execution(executionSide, executionQuantity, executionPrice,
                     buyOrder, sellOrder);
}

void Book::addOrderToBook(Order order)
{
    if (order.side == Side::Buy) {
        _buyOrders[order.price].push_back(order);
    } else {
        _sellOrders[order.price].push_back(order);
    }
}

bool Book::hasBid() const
{
    return _buyOrders.size() != 0;
}

bool Book::hasOffer() const
{
    return _sellOrders.size() != 0;
}

price_t Book::getBestBid() const
{
    if (_buyOrders.size() == 0) {
        return std::numeric_limits<price_t>::min();
    }
    return _buyOrders.begin()->first;
}

price_t Book::getBestOffer() const
{
    if (_sellOrders.size() == 0) {
        return std::numeric_limits<price_t>::max();
    }
    return _sellOrders.begin()->first;
}

Side Book::getSideForLevel(price_t price) const
{
    if (price <= getBestBid()) {
        return Side::Buy;
    }
    return Side::Sell;
}

quantity_t Book::getQuantityForLevel(price_t price) const
{
    Side side = getSideForLevel(price);
    if (side == Side::Buy) {
        auto itr = _buyOrders.find(price);
        if (itr != _buyOrders.end()) {
            quantity_t quantity = 0;
            for (const auto& order : itr->second) {
                quantity += order.quantity;
            }
            return quantity;
        }
    } else {
        auto itr = _sellOrders.find(price);
        if (itr != _sellOrders.end()) {
            quantity_t quantity = 0;
            for (const auto& order : itr->second) {
                quantity += order.quantity;
            }
            return quantity;
        }
    }
    return 0;
}

void Book::print(price_t minPrice, price_t maxPrice,
                 quantity_t maxQuantity) const
{
    auto printLevel = [&](price_t price){
        std::cout << std::setfill(' ') << std::setw(5) << price;
        std::cout << std::setw(0) << "|";
        char p = getSideForLevel(price) == Side::Buy ? 'b' : 's';
        for (quantity_t q = 0; q < maxQuantity &&
                               q < getQuantityForLevel(price); ++q) {
            std::cout << p;
        }
        std::cout << "\n";
    };
    for (price_t price = maxPrice; price >= minPrice; --price) {
        printLevel(price);
    }
}