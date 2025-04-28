#pragma once

#include "../Node.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    class TrCycle final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() override { return config; }

        uint16_t cycle_length;
        uint16_t phase_offset;

        explicit TrCycle(const uint16_t cycle_length, const uint16_t phase_offset)
            : cycle_length(cycle_length)
            , phase_offset(phase_offset)
        {
        }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            return (tick + phase_offset) % cycle_length == 0;
        }
    };

    inline const NodeConfig TrCycle::config = NodeConfig("TrCycle", "Cycle trigger", 0, 0, false, true);
}
