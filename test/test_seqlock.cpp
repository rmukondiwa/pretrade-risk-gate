#include <cassert>
#include <iostream>
#include <thread>
#include <atomic>
#include "gate/seq_lock.hpp"

using namespace gate;

struct Tick
{
    std::uint64_t a;
    std::uint64_t b;
};

int main()
{
    SeqLock<Tick> sl;
    std::atomic<bool> stop{false};

    // Writer: always writes {n, n} - both fields equal, incrementing
    std::thread writer([&] {
        for (std::uint64_t n=1; n<5'000'000; ++n)
        {
            sl.write(Tick{n,n});
        }
        stop = true;
    });

    // Reader: every read must have a == b, or we caught a torn read
    std::thread reader([&] {
        while(!stop)
        {
            Tick t = sl.read();
            assert(t.a == t.b && "TORN READ - seqlock is broken");
        }
    });

    writer.join();
    reader.join();
    std::cout << "seqlock stress test passed\n";
    return 0;
}