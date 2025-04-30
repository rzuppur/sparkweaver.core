#pragma once

#include "../Node.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    class TrRandom final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        uint32_t next_trigger = UINT32_MAX;

        TrRandom() { init(); }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick >= next_trigger || next_trigger == UINT32_MAX) {
                const uint16_t min_time    = getParam(0);
                const uint16_t max_time    = getParam(1);
                const bool     trigger_now = tick == next_trigger;
                next_trigger               = tick + random(min_time, max_time);
                return trigger_now;
            }
            return false;
        }
    };

    constexpr NodeConfig TrRandom::config = NodeConfig(
        "TrRandom",
        "Trigger random",
        0,
        0,
        false,
        true,
        {{"min_time", 1, 0xFFFF, 40}, {"max_time", 1, 0xFFFF, 400}});
}
