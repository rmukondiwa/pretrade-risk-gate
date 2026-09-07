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

class ReplayProducer
{
public:

};
int main()
{
    Loader loader;
    ReplayProducer producer;
    //RingBuffer<gate::Order> ringbuff(1024);  // not in use yet
    auto orders = loader.load("data/orders.csv");
    
    gate::SeqLock<gate::Price> livePrice;
    livePrice.write(1500000);   // seed with symbol 0's starting price

    std::atomic<bool> stop{false};

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

    std::size_t accepted =0, rejected = 0;
    for(const auto& order : orders)
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
        // std::cout << order.ts << "\t"
        //           << order.symbol_id << "\t"
        //           << (order.side == gate::Side::Buy ? "Buy" : "Sell") << "\t"
        //           << order.price << "\t"
        //           << order.qty << std::endl;
        
        // ringbuff.push(order);
        // std::cout << "order pushed!" << std::endl;
    }
    std::cout << "accepted: " << accepted << "  rejected: " << rejected << "\n";

    stop = true;
    market_data.join();
    return 0;
}