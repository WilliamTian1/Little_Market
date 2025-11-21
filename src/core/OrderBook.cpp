#include "OrderBook.h"
#include <algorithm>

namespace polysim {

std::map<double, double> OrderBook::get_snapshot() const {
    std::map<double, double> snapshot;

    // Aggregate Bids
    for (const auto& [price, orders] : bids_) {
        double total_qty = 0;
        for (const auto& order : orders) {
            total_qty += order.quantity;
        }
        if (total_qty > 0) {
            snapshot[price] = total_qty;
        }
    }

    // Aggregate Asks
    for (const auto& [price, orders] : asks_) {
        double total_qty = 0;
        for (const auto& order : orders) {
            total_qty += order.quantity;
        }
        if (total_qty > 0) {
            snapshot[price] = total_qty; // Note: In a real LOB, we might separate bids/asks, but prompt asked for a simple map
        }
    }

    return snapshot;
}

std::vector<Trade> OrderBook::AddOrder(Order order) {
    std::vector<Trade> trades;

    if (order.side == Side::BUY) {
        // Match against Asks (lowest price first)
        auto it = asks_.begin();
        while (it != asks_.end() && order.quantity > 0) {
            double best_ask_price = it->first;
            
            // If best ask is more expensive than our limit price, we can't match anymore
            if (best_ask_price > order.price) {
                break;
            }

            std::vector<Order>& orders_at_price = it->second;
            
            // Match against orders at this price level (FIFO)
            auto order_it = orders_at_price.begin();
            while (order_it != orders_at_price.end() && order.quantity > 0) {
                Order& match_order = *order_it;
                
                double trade_qty = std::min(order.quantity, match_order.quantity);
                double trade_price = match_order.price; // Trade happens at the resting order's price

                trades.push_back(Trade{
                    order.agent_id,      // buyer
                    match_order.agent_id,// seller
                    trade_price,
                    trade_qty
                });

                order.quantity -= trade_qty;
                match_order.quantity -= trade_qty;

                if (match_order.quantity <= 0) {
                    order_it = orders_at_price.erase(order_it);
                } else {
                    ++order_it;
                }
            }

            // If no orders left at this price, remove the price level
            if (orders_at_price.empty()) {
                it = asks_.erase(it);
            } else {
                ++it;
            }
        }

        // If quantity remains, add to Bids
        if (order.quantity > 0) {
            bids_[order.price].push_back(order);
        }

    } else { // Side::SELL
        // Match against Bids (highest price first)
        auto it = bids_.begin();
        while (it != bids_.end() && order.quantity > 0) {
            double best_bid_price = it->first;

            // If best bid is less than our limit price, we can't match anymore
            if (best_bid_price < order.price) {
                break;
            }

            std::vector<Order>& orders_at_price = it->second;

            // Match against orders at this price level (FIFO)
            auto order_it = orders_at_price.begin();
            while (order_it != orders_at_price.end() && order.quantity > 0) {
                Order& match_order = *order_it;

                double trade_qty = std::min(order.quantity, match_order.quantity);
                double trade_price = match_order.price; // Trade happens at the resting order's price

                trades.push_back(Trade{
                    match_order.agent_id, // buyer
                    order.agent_id,       // seller
                    trade_price,
                    trade_qty
                });

                order.quantity -= trade_qty;
                match_order.quantity -= trade_qty;

                if (match_order.quantity <= 0) {
                    order_it = orders_at_price.erase(order_it);
                } else {
                    ++order_it;
                }
            }

            // If no orders left at this price, remove the price level
            if (orders_at_price.empty()) {
                it = bids_.erase(it);
            } else {
                ++it;
            }
        }

        // If quantity remains, add to Asks
        if (order.quantity > 0) {
            asks_[order.price].push_back(order);
        }
    }

    return trades;
}

} // namespace polysim
