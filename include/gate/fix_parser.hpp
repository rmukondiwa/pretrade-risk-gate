#include <sstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

namespace gate{
    

    class FixParser
    {
        private:
            std::unordered_map<int, std::string> tokenMap;
            static constexpr char SOH = '\x01';

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

        public:
            FixParser(std::string& fixMessage)
            {
                std::vector<std::string>& tokens = splitBySOH(fixMessage, SOH);

            }
    }
}