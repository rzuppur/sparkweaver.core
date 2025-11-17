#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class MxAnd
     * @brief Acts as an AND gate for triggers.
     */
    class MxAnd final : public Node {
    public:
        static const NodeConfig config;

        explicit MxAnd(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            auto trigger = !trigger_inputs.empty();
            for (auto* trigger_input : trigger_inputs) {
                trigger = trigger_input->get(tick) && trigger;
            }
            return trigger;
        }
    };

    constexpr NodeConfig MxAnd::config =
        NodeConfig(TypeIds::MxAnd, "And", 0, MAXIMUM_CONNECTIONS, ColorOutputs::DISABLED, TriggerOutputs::ENABLED, {});
}
