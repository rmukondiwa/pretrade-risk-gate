#pragma once
#include "gate/types.hpp"
#include <atomic>

namespace gate{

    template <typename T>
    class SeqLock
    {
        private:
            std::atomic<std::uint64_t> seq; //the seq counter
            T value;                        // the protected data

        public:
            SeqLock() : seq(0)
            {

            }

            void write(const T& val) // writer thread calls this
            {
                std::uint64_t s = seq.load(std::memory_order_relaxed);
                seq.store(s+1, std::memory_order_release); // a) go odd
                std::atomic_thread_fence(std::memory_order_release);
                value = val;                               // b)
                seq.store(s+2, std::memory_order_release); // c) go even
            }

            T read() const            // reader thread(s) call this
            {
                while(true)
                {
                    std::uint64_t seq1 = seq.load(std::memory_order_acquire); // counter before
                    std::atomic_thread_fence(std::memory_order_acquire);
                    T readValue = value;            // optimistic read
                    std::atomic_thread_fence(std::memory_order_acquire);
                    std::uint64_t seq2 = seq.load(std::memory_order_acquire); // counter after

                    // clean read if: seq1 was even and unchanged
                    if((seq1 %2 == 0) && (seq1 == seq2))
                    {
                        return readValue;
                    }
                    else
                    {
                        continue;
                    }
                }
            }
    };
}