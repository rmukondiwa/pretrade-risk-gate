#pragma once

#include <iostream>
#include <vector>
#include "gates/types.hpp"
#include <string>


class Loader
{
    public:
        // Parse the whole CSV into a flat vector of Orders. All the slow work
        // (I/O, string parsing, symbol mapping, allocation) happens here
        std::vector<gate::Order> load(const std::string& filename)
        {
            std::vector<Order> orders;
            std::ifstream file(filename);

            if (!file.is_open())
            {
                std::cerr << "Failed to open: " << filename << std::endl;
                return orders;
            }

            std::string line;
            std::getline(file, line); // skip the header row

            // traverse each line, mapping it to one Order
            while (std::getline(file, line))
            {
                if (line.empty())
                {
                    continue;
                }

                gate::Order order;
                if (parseLine(line, order))
                // pull fields off the line in order, converting each string to its type
                std::getline(ss, cell, ',');  order.ts        = std::stoll(cell);
                std::getline(ss, cell, ',');  order.symbol_id = symbolToId(cell);
                std::getline(ss, cell, ',');  order.side      = (cell == "B") ? gate::Side::Buy : gate::Side::Sell;
                std::getline(ss, cell, ',');  order.price     = std::stoul(cell);
                std::getline(ss, cell, ',');  order.qty       = std::stoul(cell);

                orders.push_back(order);
            }
        }

    private:
        
}