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
                seq++;
                value = val;
                seq++;
            }

            T read() const            // reader thread(s) call this
            {
                while(true)
                {
                    std::uint64_t seq1 = seq.load(); // counter before
                    T readValue = value;            // optimistic read

                    std::uint64_t seq2 = seq.load(); // counter after

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