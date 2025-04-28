#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class MxAdd final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() override { return config; }

        MxAdd() {}

        [[nodiscard]] Color getColor(const uint32_t time, const Node* requested_by) noexcept override
        {
            Color sum = Colors::BLACK;
            for (auto* color_input : color_inputs) {
                sum = sum + color_input->getColor(time, this);
            }
            return sum;
        }
    };

    inline const NodeConfig MxAdd::config = NodeConfig("MxAdd", "Add", INPUTS_UNLIMITED, 0, true, false);
}
