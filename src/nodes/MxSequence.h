#pragma once

#include "../Node.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class MxSequence
     * @brief Sends color input to a single color output, change output with a trigger.
     */
    class MxSequence final : public Node {
        size_t   active_index     = 0;
        uint32_t cache_tick       = UINT32_MAX;
        Color    cache_color      = Colors::BLACK;
        bool     is_first_trigger = true;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        MxSequence() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick          = tick;
                bool trigger_active = false;
                for (auto* trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        trigger_active = true;
                    }
                }
                if (!color_inputs.empty() && !color_outputs.empty()) {
                    cache_color = color_inputs.at(0)->getColor(tick, this);
                    if (trigger_active) {
                        if (getParam(0) == 1) {
                            active_index = random(0, static_cast<int>(color_outputs.size()) - 1);
                        } else {
                            if (is_first_trigger) {
                                is_first_trigger = false;
                                active_index     = 0;
                            } else {
                                active_index = (active_index + 1) % color_outputs.size();
                            }
                        }
                    }
                }
            }
            return getColorOutputIndex(requested_by) == active_index ? cache_color : Colors::BLACK;
        }
    };

    constexpr NodeConfig MxSequence::config = NodeConfig(
        TypeIds::MxSequence,
        "Color sequence",
        1,
        INPUTS_UNLIMITED,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {{"random", 0, 1, 0}});
}
