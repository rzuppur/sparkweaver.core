#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class TrDelay final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        uint32_t          last_tick = UINT32_MAX;
        std::vector<bool> buffer    = {false};
        size_t            head      = 0;

        TrDelay() { init(); }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != last_tick) {
                const size_t delay_ticks = getParam(0);
                if (buffer.size() != delay_ticks) buffer.resize(delay_ticks, false);
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

    constexpr NodeConfig TrDelay::config =
        NodeConfig("TrDelay", "Delay trigger", 0, INPUTS_UNLIMITED, false, true, {{"delay_ticks", 1, 0xFF, 40}});
}
