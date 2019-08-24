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

    RandomTrader r1(exchange);
    RandomTrader r2(exchange);
    RandomTrader r3(exchange);
    RandomTrader r4(exchange);
    RandomTrader r5(exchange);
    /*
    RandomMarketOrderTrader rt1(exchange);
    RandomMarketOrderTrader rt2(exchange);
    RandomMarketOrderTrader rt3(exchange);
    */

    Curses curses;

    while (!stop) {
        exchange.tick();
        curses.clear();
        for (int i = MARKET_MAX_PRICE; i; --i ) {
            int row = MARKET_MAX_PRICE + 1 - i;
            curses.drawString(
                "-" + std::to_string(i) + "-",
                10, row);

            quantity_t quantity =
                    exchange.getBook()
                            .getQuantityForLevel(i);
            Side side = exchange.getBook()
                                  .getSideForLevel(i);
            int column = (side == Side::Buy ? 6 : 15);
            curses.drawString(
                std::to_string(quantity),
                column, row);
        }
        curses.refresh();

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }
    return 0;
}