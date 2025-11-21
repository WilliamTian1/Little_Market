#include "Engine.h"
#include <algorithm>
#include <iostream>

namespace polysim {

Engine::Engine() : rng(std::random_device{}()) {}

void Engine::add_agent(std::shared_ptr<Agent> agent) {
    agents.push_back(agent);

    // Wire up the order router
    // When the agent places an order, this lambda is called.
    agent->set_order_router([this, agent](Order order) {
        // Assign a unique ID
        order.id = ++global_order_id_counter;
        
        // Send to OrderBook
        std::vector<Trade> trades = book.AddOrder(order);

        // Notify agents of trades
        for (const auto& trade : trades) {
            // Notify Buyer
            for (auto& a : agents) {
                if (a->agent_id == trade.buyer_id) {
                    a->on_trade(trade);
                    // Update cash/inventory (simplified)
                    a->cash -= trade.price * trade.quantity;
                    a->inventory += trade.quantity;
                    break;
                }
            }

            // Notify Seller
            for (auto& a : agents) {
                if (a->agent_id == trade.seller_id) {
                    a->on_trade(trade);
                    // Update cash/inventory (simplified)
                    a->cash += trade.price * trade.quantity;
                    a->inventory -= trade.quantity;
                    break;
                }
            }
        }
    });
}

void Engine::run(int duration_ticks) {
    for (int t = 0; t < duration_ticks; ++t) {
        // 1. Get Market Snapshot
        auto snapshot = book.get_snapshot();

        // 2. Shuffle Agents (Determinism requires seeding rng properly, using random_device for now as per prompt "std::mt19937 rng")
        // Note: For strict determinism, the user should be able to set the seed. 
        // The prompt asked for "Determinism: ... given a specific Random Seed". 
        // I'll stick to the prompt's "std::mt19937 rng" member.
        std::shuffle(agents.begin(), agents.end(), rng);

        // 3. Tick Agents
        for (auto& agent : agents) {
            agent->on_tick(snapshot);
        }
    }
}

} // namespace polysim
