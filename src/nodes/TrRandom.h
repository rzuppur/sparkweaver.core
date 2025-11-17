#pragma once

#include "../NodeLink.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class TrRandom
     * @brief Outputs a trigger after a random interval following an input trigger or with no inputs continuously.
     */
    class TrRandom final : public Node {
        uint32_t next_trigger = UINT32_MAX;

    public:
        static const NodeConfig config;

        explicit TrRandom(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto min_time = getParam(0);
            const auto max_time = getParam(1);

            auto trigger = false;
            if (trigger_inputs.empty()) trigger = tick > next_trigger || next_trigger == UINT32_MAX;
            for (auto* trigger_input : trigger_inputs) {
                trigger = trigger_input->get(tick) || trigger;
            }

            if (trigger) next_trigger = tick + random(min_time, max_time);
            return tick == next_trigger;
        }
    };

    constexpr NodeConfig TrRandom::config = NodeConfig(
        TypeIds::TrRandom,
        "Random interval trigger",
        0,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"min_time", 0, PARAM_MAX_VALUE, 40}, {"max_time", 0, PARAM_MAX_VALUE, 400}});
}
