#pragma once

#include "../Node.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
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
                for (auto* trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
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

    constexpr NodeConfig TrChance::config =
        NodeConfig(TypeIds::TrChance, "Chance gate", 0, INPUTS_UNLIMITED, false, true, {{"chance", 1, 0xFFFF, 0x7FFF}});
}
