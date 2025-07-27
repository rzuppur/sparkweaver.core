#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class MxAdd final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        MxAdd() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            Color sum = Colors::BLACK;
            for (auto* color_input : color_inputs) {
                sum = sum + color_input->getColor(tick, this);
            }
            return sum;
        }
    };

    constexpr NodeConfig MxAdd::config = NodeConfig(TypeIds::MxAdd, "Mix add", INPUTS_UNLIMITED, 0, true, false, {});
}
