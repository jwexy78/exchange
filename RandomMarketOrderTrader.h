/**
 * This Trader type just randomly
 * buys/sells market orders
 */

#include "Trader.h"

class RandomMarketOrderTrader : public Trader
{
  public:
    RandomMarketOrderTrader(Exchange& exchange,
                            double _tradeChance = .1,
                            quantity_t _tradeQuantity = 10)
      : Trader(exchange), tradeChance(_tradeChance),
        tradeQuantity(_tradeQuantity) {}

    void tick() final
    {
        if (rand() / (RAND_MAX + 1.0) < tradeChance) {
            Side side = (rand() % 2) == 0 ? Side::Buy : Side::Sell;
            price_t price = side == Side::Buy ? MARKET_MAX_PRICE : MARKET_MIN_PRICE;
            submitOrder({side, tradeQuantity, price});
        }
    }

  private:
    double tradeChance;
    quantity_t tradeQuantity;
};