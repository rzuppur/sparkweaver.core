#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class SrColor final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() override { return config; }

        Color color;

        explicit SrColor(const Color color)
            : color(color)
        {
        }

        [[nodiscard]] Color getColor(const uint32_t time, const Node* requested_by) noexcept override { return color; }
    };

    inline const NodeConfig SrColor::config = NodeConfig("SrColor", "Color", 0, 0, true, false);
}
