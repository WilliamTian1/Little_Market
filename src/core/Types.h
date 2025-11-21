#pragma once

#include <cstdint>

namespace polysim {

enum class Side {
    BUY,
    SELL
};

struct Order {
    uint64_t id;
    uint64_t agent_id;
    Side side;
    double price;
    double quantity;
    // Timestamp not explicitly requested in the struct definition in prompt step 2, 
    // but was in step 1 request. I'll include it if needed, but prompt 2 didn't list it in the struct definition.
    // Re-reading prompt 2: "struct Order: uint64_t id; uint64_t agent_id; Side side; double price; double quantity;"
    // It omitted timestamp in the detailed list for step 2. I will stick to the explicit list in step 2 to be safe, 
    // or add it if I think it's critical. The prompt says "Requirements: ... struct Order: ...". I will follow that list.
};

struct Trade {
    uint64_t buyer_id;
    uint64_t seller_id;
    double price;
    double quantity;
};

} // namespace polysim
