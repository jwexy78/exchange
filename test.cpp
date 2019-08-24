#define CATCH_CONFIG_MAIN

#include "catch.h"

#include "Order.h"
#include "Book.h"
#include "Execution.h"
#include "Exchange.h"
#include "Trader.h"
#include "ManualTrader.h"

TEST_CASE("Order")
{
    SECTION("Constructor")
    {
        Order buyOrder(Side::Buy, 5, 100);
        REQUIRE(buyOrder.side == Side::Buy);
        REQUIRE(buyOrder.quantity == 5);
        REQUIRE(buyOrder.price == 100);
    }

    SECTION("Order ID")
    {
        Order order1(Side::Buy, 5, 100);
        Order order2(Side::Sell, 3, 12);
        REQUIRE(order1.id != order2.id);
    }
}

TEST_CASE("Execution")
{
    Order buyOrder(Side::Buy, 5, 100);
    Order sellOrder(Side::Sell, 5, 100);
    Execution buyReport(Side::Buy, 5, 100, buyOrder, sellOrder);
    REQUIRE(buyReport.side == Side::Buy);
    REQUIRE(buyReport.quantity == 5);
    REQUIRE(buyReport.price == 100);
    REQUIRE(buyReport.buyOrder == buyOrder);
    REQUIRE(buyReport.sellOrder == sellOrder);
}

TEST_CASE("Book")
{
    Book orderBook;

    SECTION("Non-Matching Orders")
    {
        REQUIRE(orderBook.addOrder({Side::Buy, 10, 5}).size() == 0);
        REQUIRE(orderBook.addOrder({Side::Sell, 10, 10}).size() == 0);
        REQUIRE(orderBook.addOrder({Side::Buy, 10, 5}).size() == 0);
        REQUIRE(orderBook.addOrder({Side::Sell, 10, 10}).size() == 0);
    }

    SECTION("Basic Matching (Sell Crosses)")
    {
        Order buyOrder(Side::Buy, 10, 5);
        Order sellOrder(Side::Sell, 10, 5);
        orderBook.addOrder(buyOrder);
        auto execs = orderBook.addOrder(sellOrder);
        REQUIRE(execs.size() == 1);
        auto exec = execs[0];
        REQUIRE(exec.side == Side::Sell);
        REQUIRE(exec.price == 5);
        REQUIRE(exec.quantity == 10);
        REQUIRE(exec.buyOrder == buyOrder);
        REQUIRE(exec.sellOrder == sellOrder);
    }

    SECTION("Basic Matching (Buy Crosses)")
    {
        Order buyOrder(Side::Buy, 10, 5);
        Order sellOrder(Side::Sell, 10, 5);
        orderBook.addOrder(sellOrder);
        auto execs = orderBook.addOrder(buyOrder);
        REQUIRE(execs.size() == 1);
        auto exec = execs[0];
        REQUIRE(exec.side == Side::Buy);
        REQUIRE(exec.price == 5);
        REQUIRE(exec.quantity == 10);
        REQUIRE(exec.buyOrder == buyOrder);
        REQUIRE(exec.sellOrder == sellOrder);
    }

    SECTION("Best Bid and Offer")
    {
        REQUIRE(orderBook.hasBid() == false);
        REQUIRE(orderBook.hasOffer() == false);
        orderBook.addOrder({Side::Buy, 10, 5});
        orderBook.addOrder({Side::Buy, 10, 10});
        REQUIRE(orderBook.hasBid() == true);
        REQUIRE(orderBook.hasOffer() == false);
        orderBook.addOrder({Side::Sell, 10, 15});
        orderBook.addOrder({Side::Sell, 10, 20});
        REQUIRE(orderBook.hasBid() == true);
        REQUIRE(orderBook.hasOffer() == true);
        REQUIRE(orderBook.getBestBid() == 10);
        REQUIRE(orderBook.getBestOffer() == 15);
    }

    SECTION("Side for Level")
    {
        orderBook.addOrder({Side::Buy, 10, 10});
        orderBook.addOrder({Side::Sell, 10, 11});
        REQUIRE(orderBook.getSideForLevel(10) == Side::Buy);
        REQUIRE(orderBook.getSideForLevel(11) == Side::Sell);
        REQUIRE(orderBook.getSideForLevel(5) == Side::Buy);
        REQUIRE(orderBook.getSideForLevel(20) == Side::Sell);
    }

    SECTION("Quantity for Level")
    {
        orderBook.addOrder({Side::Buy, 10, 9});
        orderBook.addOrder({Side::Buy, 10, 10});
        orderBook.addOrder({Side::Buy, 10, 10});
        orderBook.addOrder({Side::Sell, 10, 11});
        orderBook.addOrder({Side::Sell, 10, 11});
        orderBook.addOrder({Side::Sell, 10, 13});
        REQUIRE(orderBook.getQuantityForLevel(5) == 0);
        REQUIRE(orderBook.getQuantityForLevel(9) == 10);
        REQUIRE(orderBook.getQuantityForLevel(10) == 20);
        REQUIRE(orderBook.getQuantityForLevel(11) == 20);
        REQUIRE(orderBook.getQuantityForLevel(12) == 0);
        REQUIRE(orderBook.getQuantityForLevel(13) == 10);
        REQUIRE(orderBook.getQuantityForLevel(20) == 0);
    }

    SECTION("Cancel Order")
    {
        Order o1({Side::Buy, 10, 10});
        Order o2({Side::Buy, 10, 20});
        orderBook.addOrder(o1);
        orderBook.addOrder(o2);
        REQUIRE(orderBook.cancelOrder(o1.id) == true);
        REQUIRE(orderBook.getQuantityForLevel(10) == 0);
        Order o3({Side::Sell, 10, 20});
        Order o4({Side::Sell, 5, 30});
        orderBook.addOrder(o3);
        orderBook.addOrder(o4);
        REQUIRE(orderBook.cancelOrder(o2.id) == false);
        REQUIRE(orderBook.cancelOrder(o3.id) == false);
        REQUIRE(orderBook.cancelOrder(o4.id) == true);
    }
}

TEST_CASE("Exchange")
{
    Exchange exchange;
    ManualTrader trader1(exchange);
    ManualTrader trader2(exchange);
    SECTION("Basic Notification")
    {
        trader1.penOrder({Side::Buy, 10, 10});
        trader2.penOrder({Side::Sell, 10, 10});
        exchange.tick(); // tick all Traders
        exchange.tick(); // Perform first order
        exchange.tick(); // Perform second order
        REQUIRE(trader1.getMoney() == TRADER_STARTING_CAPITAL - 100);
        REQUIRE(trader2.getMoney() == TRADER_STARTING_CAPITAL + 100);
        /*
        TODO
        REQUIRE(trader2.getOrderPosition() == -10);
        REQUIRE(trader1.getMoneyPosition() == -100);
        REQUIRE(trader2.getMoneyPosition() == 100);
        REQUIRE(trader1.getSharesTraded() == 10);
        REQUIRE(trader2.getSharesTraded() == 10);
        REQUIRE(trader1.getMoneyTraded() == 100);
        REQUIRE(trader2.getMoneyTraded() == 100);
        */
    }
}