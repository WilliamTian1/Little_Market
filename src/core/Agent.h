#pragma once

#include "Types.h"
#include "OrderBook.h"
#include <string>
#include <iostream>
#include <map>
#include <functional>

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

    void set_order_router(std::function<void(const Order&)> router) {
        order_router_ = router;
    }

    void place_limit_order(Side side, double price, double quantity) {
        if (order_router_) {
            // ID will be assigned by the Engine, so we can pass 0 here
            Order order{0, agent_id, side, price, quantity}; 
            order_router_(order);
        }
    }

    // Data Members
    uint64_t agent_id;
    double cash;
    double inventory;

protected:
    std::function<void(const Order&)> order_router_;
};

} // namespace polysim
