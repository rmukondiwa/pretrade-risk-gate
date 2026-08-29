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
