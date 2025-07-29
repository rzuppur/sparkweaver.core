#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class MxSubtract final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        MxSubtract() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (!color_inputs.empty()) {
                Color sum = color_inputs.at(0)->getColor(tick, this);
                for (int i = 1; i < color_inputs.size(); i++) {
                    sum = sum - color_inputs.at(i)->getColor(tick, this);
                }
                return sum;
            }
            return Colors::BLACK;
        }
    };

    constexpr NodeConfig MxSubtract::config = NodeConfig(
        TypeIds::MxSubtract,
        "Subtract",
        INPUTS_UNLIMITED,
        0,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {});
}
