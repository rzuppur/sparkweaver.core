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
            bool set_new_trigger = trigger_inputs.empty() ? tick > next_trigger || next_trigger == UINT32_MAX : false;
            for (auto* trigger : trigger_inputs) {
                if (trigger->getTrigger(tick, this)) {
                    set_new_trigger = true;
                }
            }
            if (set_new_trigger) {
                const uint16_t min_time = getParam(0) - 1;
                const uint16_t max_time = getParam(1) - 1;
                next_trigger            = tick + random(min_time, max_time);
            }
            return tick == next_trigger;
        }
    };

    constexpr NodeConfig TrRandom::config = NodeConfig(
        TypeIds::TrRandom,
        "Random interval trigger",
        0,
        INPUTS_UNLIMITED,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"min_time", 1, PARAM_MAX_VALUE, 40}, {"max_time", 1, PARAM_MAX_VALUE, 400}});
}
