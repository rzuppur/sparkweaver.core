#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class MxOr
     * @brief Acts as an OR gate for triggers.
     */
    class MxOr final : public Node {
    public:
        static const NodeConfig config;

        explicit MxOr(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            auto trigger = false;
            for (auto* trigger_input : trigger_inputs) {
                trigger = trigger_input->get(tick) || trigger;
            }
            return trigger;
        }
    };

    constexpr NodeConfig MxOr::config =
        NodeConfig(TypeIds::MxOr, "Or", 0, MAXIMUM_CONNECTIONS, ColorOutputs::DISABLED, TriggerOutputs::ENABLED, {});
}
