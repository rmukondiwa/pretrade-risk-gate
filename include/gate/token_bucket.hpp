#pragma once
#include "gate/types.hpp"

namespace gate
{
    class TokenBucket
    {
        private:
            std::uint64_t rate, capacity;
            std::uint64_t tokens; // available tokens left
            std::uint64_t lastRefill;

            void refill(Timestamp now)
            {
                std::uint64_t elapsed = now - lastRefill;
            }

        public:

        // rate     = tokens added per second (sustained order rate)
        // capacity = max tokens the bucket can hold (burst allowance)
            TokenBucket(uint64_t rate, uint64_t capacity)
            {
                this->rate = rate;
                this->capacity = capacity;
            }

            // Try to take one token. Returns true if one was available
            // (order allowed), false if the bucket is empty (rate limited).
            bool try_take(Timestamp now)
            {
                refill(now);
                lastRefill = now;

                if(tokens<=0)
                {
                    return false;
                }

                tokens-=1;
                return true;
            }
    };
}
