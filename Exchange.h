#pragma once

#include <queue>
#include <vector>
#include <unordered_map>

#include "Book.h"
#include "Curses.h"

class Trader;

class Exchange
{
  public:
    void tick();
    void submitOrder(Trader& trader, Order order);
    void submitCancel(Trader& trader, order_id_t orderid);
    void addTrader(Trader* trader) { _traders.push_back(trader); }

    const Book& getBook() const { return _book; }
    void draw(Curses& curses);
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
            exec.buyOrder, exec.quantity, exec.price);
        _orderToTraderMap[exec.sellOrder.id]->notifyTraded(
            exec.sellOrder, exec.quantity, exec.price);
    }
}

void Exchange::submitOrder(Trader& trader, Order order)
{
    _orderQueue.push({&trader, order});
}

void Exchange::draw(Curses& curses)
{
    curses.clear();
    for (int i = MARKET_MAX_PRICE; i; --i ) {
        int row = MARKET_MAX_PRICE + 1 - i;
        curses.drawString(
            "-" + std::to_string(i) + "-",
            10, row);
        if (i > _book.getBestBid() && i < _book.getBestOffer()) {
            continue;
        }

        quantity_t quantity = _book.getQuantityForLevel(i);
        Side side = _book.getSideForLevel(i);
        int column = (side == Side::Buy ? 6 : 15);
        curses.drawString(
            std::to_string(quantity),
            column, row);
    }
    for (int i = 0; i < _traders.size() && i < 20; ++i) {
        curses.drawString(
            "Trader " + std::to_string(i) + ":", 20, i + 2);
        curses.drawString(
            "$" + std::to_string(_traders[i]->getMoney()), 30, i + 2);
        curses.drawString(
            "p" + std::to_string(_traders[i]->getShares()), 36, i + 2);
        if (_book.hasBid() && _book.hasOffer()) {
            price_t midpoint = (_book.getBestBid() + _book.getBestOffer())/2;
            curses.drawString("(~$" + 
                    std::to_string(_traders[i]->getMoney() + midpoint * _traders[i]->getShares()),
                    42, i + 2);
        }
    }

    curses.refresh();
}