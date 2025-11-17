#pragma once

#include <cstdint>

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class FxStrobe
     * @brief On trigger pulse input color for specified number of ticks.
     */
    class FxStrobe final : public Node {
        uint32_t flash_tick = UINT32_MAX;

    public:
        static const NodeConfig config;

        explicit FxStrobe(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto length = getParam(0);

            for (auto* trigger_input : trigger_inputs) {
                if (trigger_input->get(tick)) {
                    flash_tick = tick;
                }
            }

            if (color_inputs.empty()) return Colors::BLACK;
            const auto color = color_inputs.at(0)->get(tick);

            if (tick >= flash_tick && tick < flash_tick + length) return color;
            return Colors::BLACK;
        }
    };

    constexpr NodeConfig FxStrobe::config = NodeConfig(
        TypeIds::FxStrobe,
        "Strobe",
        1,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {{"flash_length", 1, PARAM_MAX_VALUE, 1}});
}
