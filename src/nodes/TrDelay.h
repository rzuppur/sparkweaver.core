#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class TrDelay
     * @brief Delays input trigger a set number of ticks.
     */
    class TrDelay final : public Node {
        uint32_t          last_tick = UINT32_MAX;
        std::vector<bool> buffer    = {false, false};
        size_t            head      = 0;

    public:
        static const NodeConfig config;

        explicit TrDelay(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto delay = getParam(0) + 1;

            if (tick != last_tick) {
                if (buffer.size() != delay) {
                    head = 0;
                    buffer.assign(delay, false);
                }
                last_tick    = tick;
                auto trigger = false;
                for (auto* trigger_input : trigger_inputs) {
                    trigger = trigger_input->get(tick) || trigger;
                }
                buffer[head] = trigger;
                head         = (head + 1) % delay;
            }

            return buffer[head];
        }
    };

    constexpr NodeConfig TrDelay::config = NodeConfig(
        TypeIds::TrDelay,
        "Delay",
        0,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"delay_ticks", 1, 0xFF, 40}});
}
