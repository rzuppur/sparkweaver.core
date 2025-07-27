#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class SrColor final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        SrColor() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            return {
                static_cast<uint8_t>(getParam(0)),
                static_cast<uint8_t>(getParam(1)),
                static_cast<uint8_t>(getParam(2))};
        }
    };

    constexpr NodeConfig SrColor::config = NodeConfig(
        TypeIds::SrColor,
        "Color",
        0,
        0,
        true,
        false,
        {{"red", 0, 0xFF, 0}, {"green", 0, 0xFF, 0}, {"blue", 0, 0xFF, 0}});
}
