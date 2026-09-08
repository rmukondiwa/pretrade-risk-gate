/*
A producer thread which reads from orders.csv and pushes the orders to the SPSC queue
It:
    opens the orders.csv file
    reads each line, parses the order
    pushes it to the SPSC queue
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "loader.hpp"
#include "gate/types.hpp"
#include "gate/ring_buffer.hpp"
#include "gate/risk_gate.hpp"
#include "gate/seq_lock.hpp"
#include <thread>
#include <atomic>

using namespace gate;
int main()
{
    Loader loader;
    auto orders = loader.load("data/orders.csv");
    
    SeqLock<Price> livePrice;
    livePrice.write(1500000);   // seed with symbol 0's starting price

    std::atomic<bool> stop{false};      // stops the market-data thread
    std::atomic<bool> producer_done{false}; // signals producer finished pushing

    // Market-data thread: wiggle symbol 0's price over time
    std::thread market_data([&] {
        gate::Price p = 1500000;
        while(!stop)
        {
            //simple simulated movement -- nudge up and down
            p+= (p%7) -3;   // crude pseudo-wiggle; replace w anything
            livePrice.write(p);
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    });

    gate::RiskConfig cfg;
    cfg.maxOrderQty = 1000;
    cfg.maxDevPercent = 5;
    cfg.referencePrices = {
    {1, 4200000},   // symbol 1 ~4.2M
    {2, 1400000},   // symbol 2 ~1.4M
    {3, 1200000},   // symbol 3 ~1.2M
    {4, 2500000},   // symbol 4 ~2.5M
    {5, 1800000},   // symbol 5 ~1.8M
    };
    gate::TokenBucket bucket{/* rate*/ 100, /*capacity*/100 };
    gate::RiskGate gate{cfg, bucket, livePrice};

    // --- The ring buffer between producer and consumer  ---
    RingBuffer<gate::Order> ring(1024);

    std::size_t accepted =0, rejected = 0;

    // --- Consumer thread: pop from ring, run the gate --
    std::thread consumer([&] {
        gate::Order order;
        while(true)
        {
            if(ring.pop(order))
            {
                gate::Decision d = gate.process(order, order.ts);
                if(d.verdict == gate::Verdict::Accept)
                {
                    ++accepted;
                }
                else
                {
                    ++rejected;
                    std::cout << "REJECT " << order.symbol_id
                        << " qty=" << order.qty
                        << " reason=" << static_cast<int>(d.reason) << "\n";
                }
            }
            else
            {
                // buffer empty: stop only if producer is done (all drained)
                if(producer_done.load(std::memory_order_acquire)) break;
                // else busy-spin
            }
        }
    });

    // -- Producer (main thread): push every order, backpressure if full --
    for(const auto& order : orders)
    {
        while(!ring.push(order))
        {
            // buffer full: spin until consumer makes room
        }
    }
    producer_done.store(true, std::memory_order_release);

    // -- Shutdown --
    consumer.join();
    stop = true;
    market_data.join();


    std::cout << "accepted: " << accepted << "  rejected: " << rejected << "\n";
    return 0;
}