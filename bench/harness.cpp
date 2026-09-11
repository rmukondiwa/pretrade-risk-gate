#include <cstdint>
#include <vector>
#include <algorithm>
#include <iostream>
#include <x86intrin.h>
#include "loader.hpp"
#include "gate/types.hpp"
#include "gate/risk_gate.hpp"
#include "gate/seq_lock.hpp"

using namespace gate;

// timing primitive
static inline std::uint64_t rdtscp_now()
{
    unsigned aux;
    return __rdtscp(&aux);
}

int main()
{
    // setup (cold path, not measured)
    Loader loader;
    auto orders = loader.load("data/orders.csv");

    SeqLock<Price> price;
    price.write(1'500'000);              // static — deterministic, no market-data thread

    RiskConfig cfg;
    cfg.maxOrderQty = 1000;
    cfg.maxDevPercent = 5;
    cfg.referencePrices = {
        {1,4200000},{2,1400000},{3,1200000},{4,2500000},{5,1800000}
    };
    TokenBucket bucket{1'000'000, 1'000'000};   // don't let rate limiting interfere with timing
    RiskGate gate{cfg, bucket, price};

    const std::size_t WARMUP     = 100'000;
    const std::size_t MEASURE    = 1'000'000;

    // sink to stop the optimizer eliding gate.process()
    volatile std::uint64_t sink = 0;

    // --- WARMUP: run orders through, discard timings ---
    for (std::size_t i = 0; i < WARMUP; ++i) {
        const Order& o = orders[i % orders.size()];
        Decision d = gate.process(o, o.ts);
        sink += static_cast<std::uint64_t>(d.verdict);
    }

    // --- MEASURE ---
    std::vector<std::uint64_t> samples;
    samples.reserve(MEASURE);            // preallocate — no allocation in the timed loop

    for(std::size_t i=0; i< MEASURE; ++i)
    {
        const Order& order = orders[i % orders.size()];

        // read the counter
        std::uint64_t t0 = rdtscp_now();

        // call gate.process(o, o.ts), keep the result
        Decision d = gate.process(order, order.ts);
        
        // read the counter again (t1)
        std::uint64_t t1 = rdtscp_now();

        // push (t1-t0) into samples
        samples.push_back(t1 - t0);

        // feed the decision into 'sink' so it isn't optimized away
        sink += static_cast<std::uint64_t>(d.verdict);
    }

    
}
