#pragma once

#include <cmath>
#include <cstdint>

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class FxBreathe
     * @brief Sinusoidal dimming of input color.
     */
    class FxBreathe final : public Node {
    public:
        static const NodeConfig config;

        explicit FxBreathe(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            constexpr static double pi            = 3.14159265358979323846;
            const uint16_t          cycle_length  = getParam(0);
            const uint16_t          phase_offset  = getParam(1);
            const float             darken_amount = static_cast<float>(getParam(2)) / 0xFF;

            if (color_inputs.empty()) return Colors::BLACK;
            const Color color = color_inputs.at(0)->get(tick);
            const auto  cycle_value =
                static_cast<float>(0.5 * (1.0 + std::sin((phase_offset + tick) * 2.0 * pi / cycle_length)));
            return color * (1 - darken_amount + cycle_value * darken_amount);
        }
    };

    constexpr NodeConfig FxBreathe::config = NodeConfig(
        TypeIds::FxBreathe,
        "Breathe",
        1,
        0,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {
            {"cycle_length", 1, PARAM_MAX_VALUE, 400},
            {"phase_offset", 0, PARAM_MAX_VALUE, 0},
            {"darken_amount", 0, 0xFF, 0xFF},
        });
}
