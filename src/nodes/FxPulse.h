#pragma once

#include <cstdint>

#include "../Node.h"

namespace SparkWeaverCore {
    class FxPulse final : public Node {
        uint32_t pulse_tick  = UINT32_MAX;
        uint32_t cache_tick  = UINT32_MAX;
        Color    cache_color = Colors::BLACK;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        FxPulse() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick = tick;

                const auto attack    = getParam(0);
                const auto sustain   = getParam(1);
                const auto decay     = getParam(2);
                const auto retrigger = getParam(3);
                const auto ignore_triggers =
                    !retrigger && tick >= pulse_tick && pulse_tick + attack + sustain + decay > tick;

                for (auto* trigger_input : trigger_inputs) {
                    if (trigger_input->getTrigger(tick, this)) {
                        if (!ignore_triggers) pulse_tick = tick;
                    }
                }

                const auto relative_tick = tick - pulse_tick;
                cache_color              = Colors::BLACK;

                if (!color_inputs.empty() && tick >= pulse_tick) {
                    const auto color = color_inputs.at(0)->getColor(tick, this);
                    if (relative_tick < attack) {
                        cache_color = color * (static_cast<float>(relative_tick) / static_cast<float>(attack));
                    } else if (relative_tick < attack + sustain) {
                        cache_color = color;
                    } else if (relative_tick < attack + sustain + decay) {
                        cache_color = color * (1 - static_cast<float>(relative_tick - attack - sustain) /
                                                       static_cast<float>(decay));
                    }
                }
            }
            return cache_color;
        }
    };

    constexpr NodeConfig FxPulse::config = NodeConfig(
        TypeIds::FxPulse,
        "Pulse",
        1,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {{"attack", 1, PARAM_MAX_VALUE, 5},
         {"sustain", 1, PARAM_MAX_VALUE, 1},
         {"decay", 1, PARAM_MAX_VALUE, 40},
         {"retrigger", 0, 1, 1}});
}
