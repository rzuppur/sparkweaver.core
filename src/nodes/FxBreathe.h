#pragma once

#include <cmath>
#include <cstdint>

#include <Node.h>
#include <bits/locale_classes.h>

namespace SparkWeaverCore {
    class FxBreathe final : public Node {
        const uint16_t cycle_length;
        const uint16_t phase_offset;
        const float    darken_amount;

        uint32_t cache_tick  = UINT32_MAX;
        Color    cache_color = Colors::BLACK;

    public:
        FxBreathe(const uint16_t cycle_length, const uint16_t phase_offset, const uint8_t darken_amount) :
            Node("FxBreathe", 1, true, 0, false),
            cycle_length(cycle_length),
            phase_offset(phase_offset),
            darken_amount(static_cast<float>(darken_amount) / 0xFF)
        {
            if (cycle_length == 0) {
                throw InvalidParameterException(name, "cycle_length must be greater than 0");
            }
            if (darken_amount < 0 || darken_amount > 0xFF) {
                throw InvalidParameterException(name, "darken_amount must be between 0 and 1");
            }
        }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick = tick;
                if (!color_inputs.empty()) {
                    const Color color = color_inputs.at(0)->getColor(tick, this);
                    const auto  cycle_value =
                        static_cast<float>(0.5 * (1.0 + std::sin((phase_offset + tick) * 2.0 * M_PI / cycle_length)));
                    cache_color = color * (1 - darken_amount + cycle_value * darken_amount);
                } else {
                    cache_color = Colors::BLACK;
                }
            }
            return cache_color;
        }
    };
}
