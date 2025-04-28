#pragma once

#include <Node.h>

namespace SparkWeaverCore {
    class MxAdd final : public Node {
    public:
        MxAdd() :
            Node(NodeConfig("MxAdd", "Add", INPUTS_UNLIMITED, 0, true, false))
        {
        }

        [[nodiscard]] Color getColor(const uint32_t time, const Node* requested_by) noexcept override
        {
            Color sum = Colors::BLACK;
            for (auto* color_input : color_inputs) {
                sum = sum + color_input->getColor(time, this);
            }
            return sum;
        }
    };
}
