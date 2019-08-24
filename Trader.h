#pragma once

#include "Order.h"
#include "Exchange.h"

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
    virtual void notifyTraded(Side side, quantity_t quantity, price_t price);

    int getMoney() const { return _money; }
    int getShares() const { return _shares; }

protected:
    void submitOrder(Order order);

private:
    Exchange& _exchange;

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
    assert(_money >= _moneyOutstanding + order.price * order.quantity);
    _exchange.submitOrder(*this, order);
}

void Trader::tick() {}

void Trader::notifyOrderAccepted(Order ord){}

void Trader::notifyTraded(Side side, quantity_t quantity, price_t price)
{
    _money += (side == Side::Buy ? -1 * quantity * price : quantity * price);
    _shares += (side == Side::Buy ? quantity : -1 * quantity);
}