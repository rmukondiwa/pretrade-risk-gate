#pragma once
#include "gate/types.hpp"
#include "gate/token_bucket.hpp"
#include "gate/seq_lock.hpp"
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
            RiskGate(const RiskConfig& cfg, TokenBucket bkt, const SeqLock<Price>& live_price) : config(cfg), bucket(bkt), livePrice(live_price){}

            Decision process (const gate::Order& order, Timestamp now)
            {
                if (!fat_finger_ok(order)) return reject(RejectReason::FatFingerPrice);
                if (!size_ok(order)) return reject(RejectReason::MaxSize);
                if(!bucket.try_take(now)) return reject(RejectReason::RateLimited);
                //TODO: position check

                return accept();
            }

        private:
            const SeqLock<Price>& livePrice;
            bool size_ok(const gate::Order& order) const
            {
                return order.qty <= config.maxOrderQty;
            }

            bool fat_finger_ok(const gate::Order& order) const
            {

                Price ref;
                if(order.symbol_id == 0)
                {
                    ref = livePrice.read();
                }
                else
                {
                    auto it = config.referencePrices.find(order.symbol_id);
                    if(it == config.referencePrices.end())
                    {
                        return true; // no ref so cant check
                    }
                    
                    ref = it->second;
                }
                
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