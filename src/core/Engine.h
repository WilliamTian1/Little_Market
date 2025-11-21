#pragma once

#include "OrderBook.h"
#include "Agent.h"
#include <vector>
#include <memory>
#include <random>

namespace polysim {

class Engine {
public:
    Engine();

    // Adds an agent to the simulation and wires up its order router.
    void add_agent(std::shared_ptr<Agent> agent);

    // Runs the simulation for a specified number of ticks.
    void run(int duration_ticks);

private:
    OrderBook book;
    std::vector<std::shared_ptr<Agent>> agents;
    std::mt19937 rng;
    uint64_t global_order_id_counter = 0;
};

} // namespace polysim
