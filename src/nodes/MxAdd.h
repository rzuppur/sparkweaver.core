#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class MxAdd final : public Node {
        uint32_t cache_color_tick = UINT32_MAX;
        Color    cache_color      = Colors::BLACK;

        uint32_t cache_trigger_tick = UINT32_MAX;
        bool     cache_trigger      = false;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        MxAdd() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_color_tick) {
                cache_color_tick = tick;
                cache_color      = Colors::BLACK;
                for (auto* color_input : color_inputs) {
                    cache_color = cache_color + color_input->getColor(tick, this);
                }
            }
            return cache_color;
        }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_trigger_tick) {
                cache_trigger_tick = tick;
                cache_trigger      = false;
                for (auto* trigger_input : trigger_inputs) {
                    if (trigger_input->getTrigger(tick, this)) {
                        cache_trigger = true;
                    }
                }
            }
            return cache_trigger;
        }
    };

    constexpr NodeConfig MxAdd::config = NodeConfig(
        TypeIds::MxAdd,
        "Add",
        MAXIMUM_CONNECTIONS,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::ENABLED,
        TriggerOutputs::ENABLED,
        {});
}
