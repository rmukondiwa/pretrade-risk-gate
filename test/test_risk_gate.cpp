#include <cassert>
#include <iostream>
#include "include/gate/risk_gate.hpp"

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

RiskConfig make_config()
{
    RiskConfig cfg;
    cfg.maxOrderQty = 1000;
    cfg.maxDevPercent = 5;
    cfg.referencePrices = {}; // symbol 0 uses the live seqlock, not the map
    return cfg;
}

void test_size_check() {
    RiskConfig cfg = make_config();
    SeqLock<Price> price;
    price.write(1'500'000);     // static, seeded - deterministic
    RiskGate gate{ cfg, TokenBucket{1000, 1000}, price };  // generous bucket, won't interfere

    Decision under = gate.process(make_order(500), 0);
    assert(under.verdict == Verdict::Accept && "500 <= 1000 should pass");

    Decision over = gate.process(make_order(1500), 0);
    assert(over.verdict == Verdict::Reject && "1500 > 1000 should reject");
    assert(over.reason == RejectReason::MaxSize && "reason should be MaxSize");
}

void test_pipeline_accepts_clean_order() {
    RiskConfig cfg = make_config();

    SeqLock<Price> price;

    price.write(1'500'000);
    RiskGate gate{ cfg, TokenBucket{1000, 1000}, price };

    Decision d = gate.process(make_order(100), 0);
    assert(d.verdict == Verdict::Accept);
    assert(d.reason == RejectReason::None);
}

void test_fat_finger_check()
{
    RiskConfig cfg = make_config();
    SeqLock<Price> price;
    price.write(1'500'000); // reference = 1.5M, band = +-75k
    RiskGate gate { cfg, TokenBucket{1000, 1000}, price};

    // buy way above the band -> reject
    Decision high_buy = gate.process(make_order(100, 15'000'000, Side::Buy), 0);
    assert(high_buy.verdict == Verdict::Reject && "buy 10x above ref should reject");
    assert(high_buy.reason == RejectReason::FatFingerPrice);

    // sell way below the band -> reject
    Decision low_sell = gate.process(make_order(100, 100'000, Side::Sell), 0);
    assert(low_sell.verdict == Verdict::Reject && "sell far below ref should reject");
    assert(low_sell.reason == RejectReason::FatFingerPrice);

    // price within band → passes fat-finger (accepted overall)
    Decision ok = gate.process(make_order(100, 1'510'000, Side::Buy), 0);
    assert(ok.verdict == Verdict::Accept && "price within band should pass");
}

int main() {
    test_size_check();
    test_pipeline_accepts_clean_order();
    test_fat_finger_check();
    std::cout << "risk_gate tests passed\n";
    return 0;
}