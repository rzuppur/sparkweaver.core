#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class MxSubtract
     * @brief Outputs the first color minus all other colors.
     */
    class MxSubtract final : public Node {
    public:
        static const NodeConfig config;

        explicit MxSubtract(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            auto color = Colors::BLACK;
            if (!color_inputs.empty()) {
                color = color_inputs.at(0)->get(tick);
                for (int i = 1; i < color_inputs.size(); i++) {
                    color = color - color_inputs.at(i)->get(tick);
                }
            }
            return color;
        }
    };

    constexpr NodeConfig MxSubtract::config = NodeConfig(
        TypeIds::MxSubtract,
        "Subtract",
        MAXIMUM_CONNECTIONS,
        0,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {});
}
