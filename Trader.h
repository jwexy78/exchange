#pragma once

#include "Order.h"
#include "Exchange.h"

/// Superclass for all "Traders" who can trade
/// on the exchange

/// Minimum price an order can be placed at
static constexpr price_t MARKET_MIN_PRICE = 1;
/// Maximum price an order can be placed at
static constexpr price_t MARKET_MAX_PRICE = 20;
/// Amount of money a trader starts out with
static constexpr price_t TRADER_STARTING_CAPITAL = 1000;
/// Number of shares a trader starts out with
static constexpr quantity_t TRADER_STARTING_POSITION = 100;

class Trader
{
public:
    /// Create a new trader and add them to the exchange
    Trader(Exchange& exchange)
      : _exchange(exchange),
        _money(TRADER_STARTING_CAPITAL), _shares(TRADER_STARTING_POSITION),
        _moneyOutstanding(0), _sharesOutstanding(0)
    {
        _exchange.addTrader(this);
    }
    virtual ~Trader() {}

    /// Tick the trader. Logic about placing orders should
    /// generally occur here
    virtual void tick();

    /// Notify the trader that an order they submitted
    /// has reached the market
    virtual void notifyOrderAccepted(Order ord);
    /// Notify the trader that an order they submitted has been
    /// (perhaps partially) filled
    virtual void notifyTraded(const Order& origOrder, quantity_t quantity, price_t price);

    /// Get how much total money the trader has
    price_t getMoney() const { return _money; }
    /// Get how many total shares the trader owns
    quantity_t getShares() const { return _shares; }
    /// Get how much free money the trader has. This is the 
    /// total amount of money less the amount comitted to submitted
    /// orders, and represents the amount that can be used for new trades.
    price_t getFreeMoney() const { return _money - _moneyOutstanding; }
    /// Same thing as `getFreeMoney` but for shares
    quantity_t getFreeShares() const { return _shares - _sharesOutstanding; }


protected:
    /// Submit an order to the exchange.
    /// Subclasses should always call this to trade
    void submitOrder(Order order);
    Exchange& _exchange;

private:
    // Total amount of money the trader has
    price_t _money;
    // Total number of shares the trader has
    quantity_t _shares;

    // Amount of money currently locked for unfilled buy orders
    price_t _moneyOutstanding;
    // Number of shares currently locked for unfilled sell orders
    quantity_t _sharesOutstanding;
};

void Trader::submitOrder(Order order)
{
    if (order.side == Side::Buy) {
        assert(getFreeMoney() >= order.price * order.quantity);
        _moneyOutstanding += order.price * order.quantity;
    } else {
        assert(getFreeShares() >= order.quantity);
        _sharesOutstanding += order.quantity;
    }
    _exchange.submitOrder(*this, order);
}

void Trader::tick() {}

void Trader::notifyOrderAccepted(Order ord){}

void Trader::notifyTraded(const Order& origOrder, quantity_t quantity, price_t price)
{
    if (origOrder.side == Side::Buy) {
        _money -= quantity * price;
        _shares += quantity;
        _moneyOutstanding -= origOrder.price * quantity;
    } else {
        _money += quantity * price;
        _shares -= quantity;
        _sharesOutstanding -= quantity;
    }
}