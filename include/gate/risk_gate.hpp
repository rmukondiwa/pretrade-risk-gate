#pragma once
#include "gate/types.hpp"
#include "gate/token_bucket.hpp"

namespace gate
{
    struct RiskConfig
    {
        Quantity maxOrderQty;
    };

    class RiskGate
    {
        public:
            RiskGate(const RiskConfig& cfg, TokenBucket bkt) : config(cfg), bucket(bkt){}

            Decision process (const gate::Order& order, Timestamp now)
            {
                if (!fat_finger_ok(order)) return reject(RejectReason::FatFingerPrice);
                if (!size_ok(order)) return reject(RejectReason::MaxSize);
                if(!bucket.try_take(now)) return reject(RejectReason::RateLimited);
                //TODO: position check

                return accept();
            }

        private:
            bool size_ok(const gate::Order& order)
            {
                return order.qty <= config.maxOrderQty;
            }

            bool fat_finger_ok(const gate::Order& order)
            {
                return true; // Stub, this will read reference data
            }
            static Decision accept()
            {
                return Decision{Verdict::Accept, RejectReason::None };
            }

            static Decision reject(gate::RejectReason r)
            {
                return Decision{Verdict::Reject, r };
            }

            RiskConfig config;
            TokenBucket bucket;

    };
}