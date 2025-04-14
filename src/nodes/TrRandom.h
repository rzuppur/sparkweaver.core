#pragma once

#include <Node.h>
#include <utils/random.h>

namespace SparkWeaverCore {
    class TrRandom final : public Node {
    public:
        uint16_t min_time;
        uint16_t max_time;
        uint32_t next_trigger;

        explicit TrRandom(const uint16_t min_time, const uint16_t max_time) :
            Node("TrRandom", 0, false, 0, true),
            min_time(min_time),
            max_time(max_time)
        {
            next_trigger = random(min_time, max_time);
        }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick >= next_trigger) {
                const bool trigger_now = tick == next_trigger;
                next_trigger           = tick + random(min_time, max_time);
                return trigger_now;
            }
            return false;
        }
    };
}
