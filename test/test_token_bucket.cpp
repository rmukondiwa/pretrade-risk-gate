#include <cassert>
#include <iostream>
#include "gate/token_bucket.hpp"

using namespace gate;

void test_starts_full_and_caps()
{
    TokenBucket b{100, 10}; //rate 100/sec, capacity 10

    // Should allow exactly 10 takes at the same instant (t=0), then fail
    for(int i = 0; i<10; i++)
    {
        assert(b.try_take(0) && "should allow up to capacity");
    }
    assert(!b.try_take(0) && "11th take at same time should fail (empty)");
}

void test_refill_over_time()
{
    TokenBucket b{100, 10}; 

    // drain it
    for(int i =0; i<10; i++)
    {
        assert(b.try_take(0));
    }
    assert(!b.try_take(0)); //empty

    // advance 10ms -> ~1 token should have refilled
    assert(b.try_take(10'000'000) && "should refill after 10 ms");
}
