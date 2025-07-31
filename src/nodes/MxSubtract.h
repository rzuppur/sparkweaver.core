#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class MxSubtract final : public Node {
        uint32_t cache_color_tick = UINT32_MAX;
        Color    cache_color      = Colors::BLACK;

        uint32_t cache_trigger_tick = UINT32_MAX;
        bool     cache_trigger      = false;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        MxSubtract() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_color_tick) {
                cache_color_tick = tick;
                cache_color      = Colors::BLACK;
                if (!color_inputs.empty()) {
                    cache_color = color_inputs.at(0)->getColor(tick, this);
                    for (int i = 1; i < color_inputs.size(); i++) {
                        cache_color = cache_color - color_inputs.at(i)->getColor(tick, this);
                    }
                }
            }
            return cache_color;
        }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_trigger_tick) {
                cache_trigger_tick = tick;
                cache_trigger      = false;
                if (!trigger_inputs.empty()) {
                    cache_trigger = trigger_inputs.at(0)->getTrigger(tick, this);
                    for (int i = 1; i < trigger_inputs.size(); i++) {
                        if (trigger_inputs.at(i)->getTrigger(tick, this)) {
                            cache_trigger = false;
                        }
                    }
                }
            }
            return cache_trigger;
        }
    };

    constexpr NodeConfig MxSubtract::config = NodeConfig(
        TypeIds::MxSubtract,
        "Subtract",
        MAXIMUM_CONNECTIONS,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::ENABLED,
        TriggerOutputs::ENABLED,
        {});
}
