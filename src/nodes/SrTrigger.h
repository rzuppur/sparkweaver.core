#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class SrTrigger final : public Node {
        uint32_t next_trigger = UINT32_MAX;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        SrTrigger() { init(); }

        void trigger(const uint32_t tick) noexcept override { next_trigger = tick; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
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
