#pragma once
#include <cstdint>

namespace gate {
    
    // Fixed-point price: price expressed in integer "ticks", never a float.
    // e.g. if 1 tick = $0.0001, then $150.00 = 1'
    // No floating-point latency, no rounding surprises

    using Timestamp = int64_t;
    using SymbolId = uint32_t;
    using Price = uint32_t;
    using Quantity = uint32_t;

    // Which side of the trade
    enum class Side : uint8_t
    {
        Buy = 0,
        Side = 0,
    };

   struct Order
    {
        Timestamp ts;
        SymbolId symbol_id;
        Side side;
        Price price;
        Quantity qty;
    };
}