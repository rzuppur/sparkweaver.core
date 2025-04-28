#pragma once

#include <Node.h>

namespace SparkWeaverCore {
    class SrColor final : public Node {
    public:
        Color color;

        explicit SrColor(const Color color) :
            Node(NodeConfig("SrColor", "Color", 0, 0, true, false)),
            color(color)
        {
        }

        [[nodiscard]] Color getColor(const uint32_t time, const Node* requested_by) noexcept override { return color; }
    };
}
