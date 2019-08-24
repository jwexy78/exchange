#pragma once

#include "Trader.h"

class ManualTrader : public Trader
{
public:
    ManualTrader(Exchange& exchange)
      : Trader(exchange) {}
    void tick() final;
    void penOrder(Order ord);

private:
    std::queue<Order> _orders;
};

void ManualTrader::tick()
{
    while (_orders.size()) {
        submitOrder(_orders.front());
        _orders.pop();
    }
}

void ManualTrader::penOrder(Order ord)
{
    _orders.push(ord);
}