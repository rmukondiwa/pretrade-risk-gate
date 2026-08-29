#include <cassert>
#include <iostream>
#include "gate/risk_gate.hpp"

using namespace gate;

// helper to build a plain order
Order make_order(Quantity qty, Price price = 1'500'000,
                 Side side = Side::Buy) {
    Order o{};
    o.ts = 0;
    o.price = price;
    o.qty = qty;
    o.side = side;
    o.symbol_id = 0;
    return o;
}

void test_size_check() {
    RiskConfig cfg{ /* max_order_qty */ 1000 };
    RiskGate gate{ cfg, TokenBucket{1000, 1000} };  // generous bucket, won't interfere

    Decision under = gate.process(make_order(500), 0);
    assert(under.verdict == Verdict::Accept && "500 <= 1000 should pass");

    Decision over = gate.process(make_order(1500), 0);
    assert(over.verdict == Verdict::Reject && "1500 > 1000 should reject");
    assert(over.reason == RejectReason::MaxSize && "reason should be MaxSize");
}

void test_pipeline_accepts_clean_order() {
    RiskConfig cfg{ 1000 };
    RiskGate gate{ cfg, TokenBucket{1000, 1000} };

    Decision d = gate.process(make_order(100), 0);
    assert(d.verdict == Verdict::Accept);
    assert(d.reason == RejectReason::None);
}

int main() {
    test_size_check();
    test_pipeline_accepts_clean_order();
    std::cout << "risk_gate tests passed\n";
    return 0;
}