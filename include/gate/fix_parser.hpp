#pragma once
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

namespace gate{
    

    class FixParser
    {
        public:
            // parse fix msg into tag map
            bool parse(std::string& fixMessage)
            {
                tokenMap.clear();
                auto tokens = splitBySOH(fixMessage);
                for(const auto& token : tokens)
                {
                    int delim = token.find(eq);
                    if(delim == std::string::npos) continue; // skip malformed token
                    int tag = std::stoi(token.substr(0, delim));
                    tokenMap[tag] = token.substr(delim+1);
                }
                return true;
            }

            const std::unordered_map<int, std::string>& fields() const { return tokenMap; }
        
        private:
            static constexpr char SOH = '\x01';
            static constexpr char eq = '=';
            std::unordered_map<int, std::string> tokenMap;

            std::vector<std::string> splitBySOH(const std::string& fixMessage) const
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

    };
}