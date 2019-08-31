/**
 * This Trader deals around a fixed midpoint
 */

#pragma once

#include "Trader.h"

class DealerTrader : public Trader
{
  public:
    DealerTrader(Exchange& exchange,
                 price_t midpoint = (MARKET_MAX_PRICE - MARKET_MIN_PRICE) / 2,
                 price_t spread = 2)
      : Trader(exchange), _midpoint(midpoint),
        _spread(spread) {}

    void tick() final
    {
        price_t buyPrice = _midpoint - _spread;
        price_t sellPrice = _midpoint + _spread;
        if (getFreeMoney() >= buyPrice) {
            submitOrder({Side::Buy, getFreeMoney()/buyPrice, buyPrice});
        }
        if (getFreeShares() > 0) {
            submitOrder({Side::Sell, getFreeShares(), sellPrice});
        }
    }

  private:
    price_t _midpoint;
    price_t _spread;
};