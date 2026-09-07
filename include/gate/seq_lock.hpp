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

            void write(const& T& val); // writer thread calls this
            T read() const;            // reader thread(s) call this
    };
}