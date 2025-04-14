#pragma once

#include <Node.h>

namespace SparkWeaverCore {
    class MxAdd final : public Node {
    public:
        MxAdd() :
            Node("MxAdd", INPUTS_UNLIMITED, true, 0, false)
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
