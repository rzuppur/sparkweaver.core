#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class TrDelay final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() override { return config; }

        uint16_t          delay_ticks;
        uint32_t          last_tick = 0;
        std::vector<bool> buffer;
        size_t            head = 0;

        explicit TrDelay(const uint16_t delay_ticks)
            : delay_ticks(delay_ticks)
        {
            buffer.resize(delay_ticks, false);
        }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != last_tick) {
                last_tick    = tick;
                buffer[head] = false;
                for (const auto& trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        buffer[head] = true;
                    }
                }
                head = (head + 1) % delay_ticks;
            }
            return buffer[head];
        }
    };

    inline const NodeConfig TrDelay::config = NodeConfig("TrDelay", "Delay trigger", 0, INPUTS_UNLIMITED, false, true);
}
