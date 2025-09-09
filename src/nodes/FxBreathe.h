#pragma once

#include <cmath>
#include <cstdint>

#include "../Node.h"

namespace SparkWeaverCore {
    class FxBreathe final : public Node {
        uint32_t cache_tick  = UINT32_MAX;
        Color    cache_color = Colors::BLACK;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        FxBreathe() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            constexpr static double pi            = 3.14159265358979323846;
            const uint16_t          cycle_length  = getParam(0);
            const uint16_t          phase_offset  = getParam(1);
            const float             darken_amount = static_cast<float>(getParam(2)) / 0xFF;

            if (tick != cache_tick) {
                cache_tick = tick;
                if (!color_inputs.empty()) {
                    const Color color       = color_inputs.at(0)->getColor(tick, this);
                    const auto  cycle_value = static_cast<float>(
                        0.5 * (1.0 + std::sin((phase_offset + tick) * 2.0 * pi / cycle_length)));
                    cache_color = color * (1 - darken_amount + cycle_value * darken_amount);
                } else {
                    cache_color = Colors::BLACK;
                }
            }
            return cache_color;
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
