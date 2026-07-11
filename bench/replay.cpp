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

class ReplayProducer
{
public:

// Reads a CSV file and returns a vector of rows, where each row is a vector of strings (cells)
    std::vector<std::vector<std::string>> readCSV(const std::string& filename)
    {
        std::vector<std::vector<std::string>> data;
        std::ifstream file(filename);

        if (!file.is_open())
        {
            std::cerr << "Failed to open: " << filename << std::endl;
            return data;
        }

        std::string line;
        while (std::getline(file, line))
        {
            std::vector<std::string> row;
            std::stringstream ss(line);
            std::string cell;

            while(std::getline(ss, cell, ','))
            {
                row.push_back(cell);
            }

            data.push_back(row);
        }

        file.close();
        return data;
    }
};

int main()
{
    ReplayProducer producer;
    auto data = producer.readCSV("data/orders.csv");

    for(const auto& row : data)
    {
        for(const auto& cell : row)
        {
            std::cout << cell << "\t";
        }
        std::cout << std::endl;
    }

    return 0;
}