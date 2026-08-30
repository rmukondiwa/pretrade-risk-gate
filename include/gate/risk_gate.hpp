#pragma once
#include "gate/types.hpp"
#include "gate/token_bucket.hpp"
#include <unordered_map>

namespace gate
{
    struct RiskConfig
    {
        Quantity maxOrderQty;
        std::unordered_map<SymbolId, Price> referencePrices; // per-symbol
        uint32_t maxDevPercent;
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
            bool size_ok(const gate::Order& order) const
            {
                return order.qty <= config.maxOrderQty;
            }

            bool fat_finger_ok(const gate::Order& order) const
            {
                auto it = config.referencePrices.find(order.symbol_id);
                if(it == config.referencePrices.end())
                {
                    return true; // no ref so cant check
                }
                
                Price ref = it->second;
                Price band = ref * config.maxDevPercent / 100;

                if(order.side == Side::Buy)
                {
                    // reject if buying too far ABOVE market
                    return order.price <= ref + band;
                }
                else
                {
                    return order.price >= ref - band;
                }

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