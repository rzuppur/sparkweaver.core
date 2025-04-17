#pragma once

#include <Node.h>
#include <utils/random.h>

namespace SparkWeaverCore {
    class TrCycle final : public Node {
    public:
        uint16_t cycle_length;
        uint16_t phase_offset;

        explicit TrCycle(const uint16_t cycle_length, const uint16_t phase_offset) :
            Node("TrCycle", 0, false, 0, true),
            cycle_length(cycle_length),
            phase_offset(phase_offset)
        {
        }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            return (tick + phase_offset) % cycle_length == 0;
        }
    };
}
