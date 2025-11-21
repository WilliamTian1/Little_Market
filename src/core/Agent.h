#pragma once

#include "Types.h"
#include "OrderBook.h"
#include <string>
#include <iostream>
#include <map>

namespace polysim {

class Agent {
public:
    Agent(uint64_t id, double initial_cash, double initial_inventory)
        : agent_id(id), cash(initial_cash), inventory(initial_inventory) {}

    virtual ~Agent() = default;

    // The Interface
    virtual void on_tick(const std::map<double, double>& market_snapshot) = 0;
    virtual void on_trade(const Trade& trade) = 0;

    // Helper Methods
    void log(const std::string& message) {
        std::cout << "[Agent " << agent_id << "] " << message << std::endl;
    }

    // Data Members
    uint64_t agent_id;
    double cash;
    double inventory;
};

} // namespace polysim
