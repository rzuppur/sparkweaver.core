#pragma once

#include "../Node.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class TrChance
     * @brief Outputs input trigger based on set probability
     */
    class TrChance final : public Node {
        uint32_t cache_tick    = UINT32_MAX;
        bool     cache_trigger = false;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        TrChance() { init(); }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick    = tick;
                cache_trigger = false;
                for (auto* trigger_input : trigger_inputs) {
                    if (trigger_input->getTrigger(tick, this)) {
                        cache_trigger = true;
                    }
                }
                if (random(0, 0xFFFF) > getParam(0)) {
                    cache_trigger = false;
                }
            }
            return cache_trigger;
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
