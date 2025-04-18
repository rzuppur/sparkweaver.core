#pragma once

#include <Node.h>

namespace SparkWeaverCore {
    class MxSubtract final : public Node {
    public:
        MxSubtract() :
            Node("MxSubtract", INPUTS_UNLIMITED, true, 0, false)
        {
        }

        [[nodiscard]] Color getColor(const uint32_t time, const Node* requested_by) noexcept override
        {
            if (!color_inputs.empty()) {
                Color sum = color_inputs.at(0)->getColor(time, this);
                for (int i = 1; i < color_inputs.size(); i++) {
                    sum = sum - color_inputs.at(i)->getColor(time, this);
                }
                return sum;
            }
            return Colors::BLACK;
        }
    };
}
