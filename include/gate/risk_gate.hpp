#pragma once
#include "gate/types.hpp"

namespace gate
{

    class RiskGate 
    {
        public:
            Decision process (gate::Order& order)
            {

            }

        private:
            static Decision accept()
            {
                return Decision{Verdict::Accept, RejectReason::None };
            }

            static Decision reject(gate::RejectReason r)
            {
                return Decision{Verdict::Reject, r };
            }

    }


}