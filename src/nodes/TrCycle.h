#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class TrCycle final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        TrCycle() { init(); }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            const uint16_t cycle_length = getParam(0);
            const uint16_t phase_offset = getParam(1);
            return (tick + phase_offset) % cycle_length == 0;
        }
    };

    constexpr NodeConfig TrCycle::config = NodeConfig(
        TypeIds::TrCycle,
        "Cycle trigger",
        0,
        0,
        false,
        true,
        {{"cycle_length", 1, 0xFFFF, 40}, {"phase_offset", 0, 0xFFFF, 0}});
}
