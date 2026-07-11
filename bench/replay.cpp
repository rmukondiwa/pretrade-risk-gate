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
#include "gate/types.hpp"

class ReplayProducer
{
public:

    // Maps a ticker string ("AAPL") to a numeric SymbolId, assigning a new id
    // the first time a symbol is seen. symbol_id is an integer, never a string.
    gate::SymbolId symbolToId(const std::string& symbol)
    {
        auto it = symbolIds.find(symbol);
        if (it != symbolIds.end())
            return it->second;
        gate::SymbolId id = static_cast<gate::SymbolId>(symbolIds.size());
        symbolIds[symbol] = id;
        return id;
    }

    // Reads the CSV and returns one gate::Order per data row.
    std::vector<gate::Order> readCSV(const std::string& filename)
    {
        std::vector<gate::Order> orders;
        std::ifstream file(filename);



        

        file.close();
        return orders;
    }

private:
    std::unordered_map<std::string, gate::SymbolId> symbolIds;
};

int main()
{
    ReplayProducer producer;
    auto orders = producer.readCSV("data/orders.csv");

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