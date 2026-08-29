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

class ReplayProducer
{
public:

};
int main()
{
    Loader loader;
    ReplayProducer producer;
    RingBuffer<gate::Order> ringbuff(1024);
    auto orders = loader.load("data/orders.csv");
    gate::RiskConfig cfg{/*max order qty*/ 50};
    gate::TokenBucket bucket{/* rate*/ 10, /*capacity*/10 };
    gate::RiskGate gate{cfg, bucket};

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
    return 0;
}