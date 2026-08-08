#pragma once
#include "gate/types.hpp"
#include <algorithm>

namespace gate
{
    class TokenBucket
    {
        private:
            std::uint64_t rate, capacity;
            std::uint64_t tokens; // available tokens left
            Timestamp lastRefill;
            static constexpr std::uint64_t NANOS_PER_SEC = 1'000'000'000;

            void refill(Timestamp now)
            {
                std::uint64_t elapsed = now - lastRefill;
                std::uint64_t newTokens = elapsed*rate / NANOS_PER_SEC;
                if(newTokens>0)
                {
                    tokens = std::min(tokens+newTokens, capacity);
                    lastRefill = now;
                }
            }

        public:

        // rate     = tokens added per second (sustained order rate)
        // capacity = max tokens the bucket can hold (burst allowance)
            TokenBucket(uint64_t rate, uint64_t capacity, uint64_t tokens)
            {
                this->rate = rate;
                this->capacity = capacity;
                this->tokens = tokens;
                lastRefill = 0;
            }

            // Try to take one token. Returns true if one was available
            // (order allowed), false if the bucket is empty (rate limited).
            bool try_take(Timestamp now)
            {
                refill(now);

                if(tokens==0)
                {
                    return false;
                }

                tokens-=1;
                return true;
            }
    };
}
