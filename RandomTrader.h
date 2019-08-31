/**
 * This Trader type just randomly
 * makes trades
 */

#include "Trader.h"

class RandomTrader : public Trader
{
  public:
    RandomTrader(Exchange& exchange,
                            double _tradeChance = .1,
                            quantity_t _maxQuantity = 10,
                            price_t _maxPrice = MARKET_MAX_PRICE)
      : Trader(exchange), tradeChance(_tradeChance),
        maxQuantity(_maxQuantity),
        maxPrice(_maxPrice) {}

    void tick() final
    {
        if (rand() / (RAND_MAX + 1.0) < tradeChance) {
            Side side = (rand() % 2) == 0 ? Side::Buy : Side::Sell;
            price_t price = (rand() % maxPrice) + 1;
            quantity_t quantity = (rand() % maxQuantity) + 1;
            if (side == Side::Buy && price * quantity > getFreeMoney()) {
                return;
            }
            if (side == Side::Sell && quantity > getFreeShares()) {
                return;
            }
            submitOrder({side, quantity, price});
        }
    }

  private:
    double tradeChance;
    quantity_t maxQuantity;
    price_t maxPrice;
};