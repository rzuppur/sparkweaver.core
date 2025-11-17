#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class SrColor
     * @brief Outputs a static color.
     */
    class SrColor final : public Node {
    public:
        static const NodeConfig config;

        explicit SrColor(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            const uint8_t red   = getParam(0);
            const uint8_t green = getParam(1);
            const uint8_t blue  = getParam(2);

            return {red, green, blue};
        }
    };

    constexpr NodeConfig SrColor::config = NodeConfig(
        TypeIds::SrColor,
        "Color",
        0,
        0,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {{"red", 0, 0xFF, 0}, {"green", 0, 0xFF, 0}, {"blue", 0, 0xFF, 0}});
}
