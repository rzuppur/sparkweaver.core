#pragma once

#include <Node.h>

namespace SparkWeaverCore {
    class SrColor final : public Node {
    public:
        Color color;

        explicit SrColor(const Color color) :
            Node("SrColor", 0, true, 0, false),
            color(color)
        {
        }

        [[nodiscard]] Color getColor(const uint32_t time, const Node* requested_by) noexcept override { return color; }
    };
}
