// Seqlock: single-writer / multi-reader. Fence placement follows
// rigtorp's C++11 Seqlock and Boehm's seqlock memory-model analysis.

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
                std::atomic_thread_fence(std::memory_order_release);
                seq.store(s+2, std::memory_order_release); // c) go even
            }

            T read() const            // reader thread(s) call this
            {
                T copy;
                std::uint64_t seq0, seq1;
                do {
                    seq0 = seq.load(std::memory_order_acquire);   // counter before (acquire carries front edge)
                    copy = value;                                  // optimistic data read
                    std::atomic_thread_fence(std::memory_order_acquire);  // ONE fence, AFTER the read
                    seq1 = seq.load(std::memory_order_relaxed);   // counter after (relaxed — fence did the work)
                } while (seq0 != seq1 || (seq0 & 1));
                return copy;

            }
    };
}