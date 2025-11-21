#include "OrderBook.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace polysim;

void test_matching() {
    OrderBook book;

    // 1. Add a SELL order: 10 shares @ $100
    Order sell_order{1, 101, Side::SELL, 100.0, 10.0};
    auto trades1 = book.AddOrder(sell_order);

    // Assertions for Sell Order
    assert(trades1.empty()); // Should be no trades yet
    assert(book.getAsks().size() == 1); // Should be in the book
    assert(book.getAsks().at(100.0).size() == 1);
    assert(book.getAsks().at(100.0)[0].quantity == 10.0);
    std::cout << "Sell order added successfully." << std::endl;

    // 2. Add a BUY order: 10 shares @ $100
    Order buy_order{2, 102, Side::BUY, 100.0, 10.0};
    auto trades2 = book.AddOrder(buy_order);

    // Assertions for Buy Order (Match)
    assert(trades2.size() == 1);
    const auto& trade = trades2[0];
    
    assert(trade.buyer_id == 102);
    assert(trade.seller_id == 101);
    assert(std::abs(trade.price - 100.0) < 1e-9);
    assert(std::abs(trade.quantity - 10.0) < 1e-9);
    
    // Book should be empty now
    assert(book.getAsks().empty());
    assert(book.getBids().empty());

    std::cout << "Trade executed successfully: " << trade.quantity << " @ " << trade.price << std::endl;
}

int main() {
    try {
        test_matching();
        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
