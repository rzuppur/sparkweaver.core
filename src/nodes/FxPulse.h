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
                for (auto* trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        pulse_tick = tick;
                    }
                }

                const auto attack  = getParam(0);
                const auto sustain = getParam(1);
                const auto decay   = getParam(2);

                const auto relative_tick = tick - pulse_tick;
                const auto color         = color_inputs.at(0)->getColor(tick, this);
                cache_color              = Colors::BLACK;

                if (!color_inputs.empty() && tick >= pulse_tick) {
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
        "FxPulse",
        "Pulse effect",
        1,
        INPUTS_UNLIMITED,
        true,
        false,
        {{"attack", 1, 0xFFFF, 5}, {"sustain", 1, 0xFFFF, 1}, {"decay", 1, 0xFFFF, 40}});
}
