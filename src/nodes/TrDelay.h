#pragma once

#include <Node.h>

namespace SparkWeaverCore {
    class TrDelay final : public Node {
    public:
        uint16_t          delay_time;
        uint32_t          last_tick = 0;
        std::vector<bool> buffer;
        size_t            head = 0;

        explicit TrDelay(const uint16_t delay_time) :
            Node("TrDelay", 0, false, INPUTS_UNLIMITED, true),
            delay_time(delay_time)
        {
            buffer.resize(delay_time, false);
        }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != last_tick) {
                last_tick    = tick;
                buffer[head] = false;
                for (const auto& trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        buffer[head] = true;
                        break;
                    }
                }
                head = (head + 1) % delay_time;
            }
            return buffer[head];
        }
    };
}
