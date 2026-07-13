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

    for(const auto& order : orders)
    {
        std::cout << order.ts << "\t"
                  << order.symbol_id << "\t"
                  << (order.side == gate::Side::Buy ? "Buy" : "Sell") << "\t"
                  << order.price << "\t"
                  << order.qty << std::endl;
        
        ringbuff.push(order);
        std::cout << "order pushed!" << std::endl;
    }

    return 0;
}