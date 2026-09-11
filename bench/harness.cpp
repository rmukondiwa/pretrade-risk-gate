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
    const std::size_t BATCH       = 1000;
    const std::size_t NUM_BATCHES = 1000;    // BATCH * NUM_BATCHES = 1M total calls

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
    samples.reserve(NUM_BATCHES);            // preallocate — no allocation in the timed loop

    for(std::size_t b=0; b< NUM_BATCHES; ++b)
    {
       std::uint64_t t0 = rdtscp_now();
        _mm_lfence();

        for (std::size_t j = 0; j < BATCH; ++j) {
            const Order& order = orders[(b * BATCH + j) % orders.size()];
            Decision d = gate.process(order, order.ts);
            sink += static_cast<std::uint64_t>(d.verdict);
        }

        _mm_lfence();
        std::uint64_t t1 = rdtscp_now();

        samples.push_back(static_cast<double>(t1 - t0) / BATCH);
    }

    // report percentiles
    std::sort(samples.begin(), samples.end());
    auto pct = [&](double p) {
        return samples[static_cast<std::size_t>(p * (samples.size() - 1))];
    };

    std::cout << "batches: " << samples.size() << " (BATCH=" << BATCH << ")\n";
    std::cout << "min:   " << samples.front()  << " cycles/op\n";
    std::cout << "p50:   " << pct(0.50)  << " cycles/op\n";
    std::cout << "p99:   " << pct(0.99)  << " cycles/op\n";
    std::cout << "p99.9: " << pct(0.999) << " cycles/op\n";
    std::cout << "max:   " << samples.back()   << " cycles/op\n";
    std::cout << "(sink=" << sink << ")\n";

    return 0;
}
