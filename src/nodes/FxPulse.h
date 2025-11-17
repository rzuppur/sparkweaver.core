#pragma once

#include <cstdint>

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class FxPulse
     * @brief On trigger pulse input color with linear attack, sustain and decay.
     */
    class FxPulse final : public Node {
        uint32_t pulse_tick = UINT32_MAX;

    public:
        static const NodeConfig config;

        explicit FxPulse(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto attack           = getParam(0);
            const auto sustain          = getParam(1);
            const auto decay            = getParam(2);
            const auto retrigger        = getParam(3);
            const auto relative_tick    = tick - pulse_tick;
            const auto retrigger_ignore = !retrigger && tick >= pulse_tick && attack + sustain + decay > relative_tick;

            for (auto* trigger_input : trigger_inputs) {
                if (trigger_input->get(tick)) {
                    if (!retrigger_ignore) pulse_tick = tick;
                }
            }

            if (color_inputs.empty()) return Colors::BLACK;
            const auto color = color_inputs.at(0)->get(tick);

            if (tick < pulse_tick || tick >= pulse_tick + attack + sustain + decay) return Colors::BLACK;
            if (relative_tick < attack) return color * (static_cast<float>(relative_tick) / static_cast<float>(attack));
            if (relative_tick < attack + sustain) return color;
            if (relative_tick < attack + sustain + decay)
                return color * (1 - static_cast<float>(relative_tick - attack - sustain) / static_cast<float>(decay));
            return Colors::BLACK;
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
