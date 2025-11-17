#pragma once

#include "../NodeLink.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class TrChance
     * @brief Outputs input trigger based on set probability.
     */
    class TrChance final : public Node {
        uint32_t last_tick  = UINT32_MAX;
        bool     last_value = false;

    public:
        static const NodeConfig config;

        explicit TrChance(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto chance = getParam(0);

            if (tick != last_tick) {
                last_tick    = tick;
                auto trigger = false;
                for (auto* trigger_input : trigger_inputs) {
                    trigger = trigger_input->get(tick) || trigger;
                }
                last_value = trigger && chance > random(0, PARAM_MAX_VALUE - 1);
            }

            return last_value;
        }
    };

    constexpr NodeConfig TrChance::config = NodeConfig(
        TypeIds::TrChance,
        "Stochastic gate",
        0,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"chance", 1, PARAM_MAX_VALUE, 0x7FFF}});
}
