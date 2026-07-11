#pragma once

#include <iostream>
#include <vector>
#include "gate/types.hpp"
#include <string>


class Loader
{
    public:
        // Parse the whole CSV into a flat vector of Orders. All the slow work
        // (I/O, string parsing, symbol mapping, allocation) happens here
        std::vector<gate::Order> load(const std::string& filename)
        {
            std::vector<gate::Order> orders;
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
                if (parseLine(line, order)) // skip malformed rows instead of crashing
                    orders.push_back(order);

            }
            return orders;
        }

        // Exposed so the rest of the system can resolve the same ids after loading.
        gate::SymbolId idOf(const std::string& symbol) const
        {
            auto it = symbolIds_.find(symbol);
            return it == symbolIds_.end() ? gate::SymbolId(-1) : it->second;
        }
    private:
        // Turn one "ts,symbol,side,prince,qty" line into an Order
        // Return false (and leaves order untouched) if the line is malformed
        bool parseLine(const std::string& line, gate::Order& order)
        {
            std::stringstream ss(line);
            std::string cell;

            try
            {
                std::getline(ss, cell, ',');  order.ts        = std::stoll(cell);
                std::getline(ss, cell, ',');  order.symbol_id = internalSymbol(cell);
                std::getline(ss, cell, ',');  order.side      = (cell == "B") ? gate::Side::Buy : gate::Side::Sell;
                std::getline(ss, cell, ',');  order.price     = std::stoul(cell);
                std::getline(ss, cell, ',');  order.qty       = std::stoul(cell);
            }
            catch (const std::exception& e)
            {
                std::cerr << "Skipping bad line: " << line << "  (" << e.what() << ")\n";
                return false;
            }
            return true;
        }

        // "AAPL" -> stable integer id, assigned on first sight.
        gate::SymbolId internalSymbol(const std::string& symbol)
        {
            auto it = symbolIds_.find(symbol);
            if (it!=symbolIds_.end())
            {
                return it->second;
            }
            gate::SymbolId id = static_cast<gate::SymbolId>(symbolIds_.size());
            symbolIds_[symbol] = id;
            return id;
        }

        std::unordered_map<std::string, gate::SymbolId> symbolIds_;
};