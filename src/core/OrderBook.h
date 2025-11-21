#pragma once

#include "Types.h"
#include <vector>
#include <map>
#include <functional>

namespace polysim {

class OrderBook {
public:
    OrderBook() = default;

    // Adds an order to the book, attempting to match it first.
    // Returns a vector of trades executed.
    std::vector<Trade> AddOrder(Order order);

    // Returns a simplified snapshot of the book (Price Level -> Total Quantity)
    std::map<double, double> get_snapshot() const;

    // Getters for inspection (useful for testing)
    const std::map<double, std::vector<Order>, std::greater<double>>& getBids() const { return bids_; }
    const std::map<double, std::vector<Order>, std::less<double>>& getAsks() const { return asks_; }

private:
    // Bids: Highest price first
    std::map<double, std::vector<Order>, std::greater<double>> bids_;
    
    // Asks: Lowest price first
    std::map<double, std::vector<Order>, std::less<double>> asks_;
};

} // namespace polysim
