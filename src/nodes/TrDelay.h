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
            const auto delay = getParam(0) + 1;
            if (tick != last_tick) {
                if (buffer.size() != delay) buffer.resize(delay + 2, false);
                last_tick    = tick;
                buffer[head] = false;
                for (const auto& trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        buffer[head] = true;
                    }
                }
                head = (head + 1) % delay;
            }
            return buffer[head];
        }
    };

    constexpr NodeConfig TrDelay::config =
        NodeConfig(TypeIds::TrDelay, "Delay", 0, MAXIMUM_CONNECTIONS, ColorOutputs::DISABLED, TriggerOutputs::ENABLED, {{"delay_ticks", 1, 0xFF, 40}});
}
