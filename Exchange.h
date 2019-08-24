#pragma once

#include <queue>
#include <vector>
#include <unordered_map>

#include "Book.h"

class Trader;

class Exchange
{
  public:
    void tick();
    void submitOrder(Trader& trader, Order order);
    void submitCancel(Trader& trader, order_id_t orderid);
    void addTrader(Trader* trader) { _traders.push_back(trader); }

    const Book& getBook() const { return _book; }
  private:
    Book _book;
    std::vector<Trader*> _traders;
    std::queue<std::pair<Trader*,Order>> _orderQueue;
    std::unordered_map<order_id_t,Trader*> _orderToTraderMap;
};

#include "Trader.h"

void Exchange::tick()
{
    // If there are no orders to process,
    // tick all the traders
    if (_orderQueue.size() == 0) {
        for (Trader *trader : _traders) {
            trader->tick();
        }
        return;
    }
    // Otherwise, process the first order
    std::pair<Trader*,Order> next = _orderQueue.front();
    _orderQueue.pop();
    _orderToTraderMap.emplace(next.second.id, next.first);
    next.first->notifyOrderAccepted(next.second);
    std::vector<Execution> execs = _book.addOrder(next.second);
    for (const auto& exec : execs) {
        _orderToTraderMap[exec.buyOrder.id]->notifyTraded(
            Side::Buy, exec.quantity, exec.price);
        _orderToTraderMap[exec.sellOrder.id]->notifyTraded(
            Side::Sell, exec.quantity, exec.price);
    }
}

void Exchange::submitOrder(Trader& trader, Order order)
{
    _orderQueue.push({&trader, order});
}