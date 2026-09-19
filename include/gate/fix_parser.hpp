#pragma once
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

namespace gate{
    

    class FixParser
    {
        private:
            static constexpr char SOH = '\x01';
            static constexpr char eq = '=';

            std::vector<std::string> splitBySOH(std::string& fixMessage, char SOH)
            {
                std::stringstream ss(fixMessage);
                std::string token;
                std::vector<std::string> tokens;

                while(getline(ss, token, SOH))
                {
                    if(!token.empty())
                    {
                        tokens.push_back(token);
                    }
                }

                return tokens;
            }

            void splitIntoMap(std::vector<std::string>& tokens, char eq)
            {
                for(auto& token : tokens)
                {
                    int delim = token.find(eq);
                    tokenMap[std::stoi(token.substr(0, delim))] = token.substr(delim+1);
                }
            }

        public:
            std::unordered_map<int, std::string> tokenMap;
            FixParser(std::string& fixMessage)
            {
                std::vector<std::string> tokens = splitBySOH(fixMessage, SOH);
                splitIntoMap(tokens, eq);
            }
    };
}