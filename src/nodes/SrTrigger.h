#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class SrTrigger
     * @brief Outputs external triggers.
     */
    class SrTrigger final : public Node {
        uint32_t next_trigger = UINT32_MAX;

    public:
        static const NodeConfig config;

        explicit SrTrigger(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        void trigger(const uint32_t tick) noexcept override { next_trigger = tick; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            return tick == next_trigger;
        }
    };

    constexpr NodeConfig SrTrigger::config = NodeConfig(
        TypeIds::SrTrigger,
        "External trigger",
        0,
        0,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"id", 0, 0xFF, 0}});
}
