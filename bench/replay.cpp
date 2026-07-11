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

class ReplayProducer
{
public:

};
int main()
{
    Loader loader;
    ReplayProducer producer;
    auto orders = loader.load("data/orders.csv");

    for(const auto& order : orders)
    {
        std::cout << order.ts << "\t"
                  << order.symbol_id << "\t"
                  << (order.side == gate::Side::Buy ? "Buy" : "Sell") << "\t"
                  << order.price << "\t"
                  << order.qty << std::endl;
    }

    return 0;
}