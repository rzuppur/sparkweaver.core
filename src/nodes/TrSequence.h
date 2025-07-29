#pragma once

#include "../Node.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class TrSequence
     * @brief Sends incoming triggers to a single trigger output, either sequentially or randomly.
     */
    class TrSequence final : public Node {
        size_t   active_index     = 0;
        uint32_t cache_tick       = UINT32_MAX;
        bool     cache_trigger    = false;
        bool     is_first_trigger = true;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        TrSequence() { init(); }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick    = tick;
                cache_trigger = false;
                for (auto* trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        cache_trigger = true;
                    }
                }
                if (cache_trigger && !trigger_outputs.empty()) {
                    if (getParam(0) == 1) {
                        active_index = random(0, static_cast<int>(trigger_outputs.size()) - 1);
                    } else {
                        if (is_first_trigger) {
                            is_first_trigger = false;
                            active_index     = 0;
                        } else {
                            active_index = (active_index + 1) % trigger_outputs.size();
                        }
                    }
                }
            }
            return cache_trigger && getTriggerOutputIndex(requested_by) == active_index;
        }
    };

    constexpr NodeConfig TrSequence::config = NodeConfig(
        TypeIds::TrSequence,
        "Trigger sequence",
        0,
        INPUTS_UNLIMITED,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"random", 0, 1, 0}});
}
