#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class MxAdd
     * @brief Outputs the sum of all colors.
     */
    class MxAdd final : public Node {
    public:
        static const NodeConfig config;

        explicit MxAdd(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            auto color = Colors::BLACK;
            for (auto* color_input : color_inputs) {
                color = color + color_input->get(tick);
            }
            return color;
        }
    };

    constexpr NodeConfig MxAdd::config =
        NodeConfig(TypeIds::MxAdd, "Add", MAXIMUM_CONNECTIONS, 0, ColorOutputs::ENABLED, TriggerOutputs::DISABLED, {});
}
