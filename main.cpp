#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <csignal>

#include "Book.h"
#include "Exchange.h"
#include "RandomMarketOrderTrader.h"
#include "Curses.h"
#include "RandomTrader.h"
#include "DealerTrader.h"
#include "SpreadTrader.h"

bool stop = false;

void signalHandler(int signal)
{
    stop = true;
}

int main()
{
    signal(SIGINT, signalHandler);

    srand(time(NULL));
    Exchange exchange;

    SpreadTrader s1(exchange);
    DealerTrader d1(exchange);
    DealerTrader d2(exchange, (MARKET_MAX_PRICE - MARKET_MIN_PRICE) / 2, 1);
    constexpr int NUM_RANDOM_TRADERS = 1000;
    RandomTrader* randomTraders[NUM_RANDOM_TRADERS];
    for (int i = 0; i < NUM_RANDOM_TRADERS; ++i) {
        randomTraders[i] = new RandomTrader(exchange);
    }

    Curses curses;

    while (!stop) {
        exchange.tick();
        exchange.draw(curses);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(25)
        );
    }
    return 0;
}