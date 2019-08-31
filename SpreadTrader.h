/**
 * This Trader follows the midpoint
 */

#pragma once

#include "Trader.h"

class SpreadTrader : public Trader
{
  public:
    SpreadTrader(Exchange& exchange)
      : Trader(exchange) {}

    void tick() final
    {
        const Book& book = _exchange.getBook();
        if (book.hasBid() && book.hasOffer()) {
            price_t bid = book.getBestBid();
            price_t ask = book.getBestOffer();
            price_t mid = (bid + ask) / 2;
            // Buy at the midpoint, sell at midpoint + 1
            if (getFreeMoney() >= mid) {
                submitOrder({Side::Buy, getFreeMoney()/mid, mid});
            }
            if (getFreeShares() > 0) {
                submitOrder({Side::Sell, getFreeShares(), mid + 1});
            }
        }
    }
};