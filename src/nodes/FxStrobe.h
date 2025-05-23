#pragma once

#include <cstdint>

#include "../Node.h"

namespace SparkWeaverCore {
    class FxStrobe final : public Node {
        uint32_t flash_tick  = UINT32_MAX;
        uint32_t cache_tick  = UINT32_MAX;
        Color    cache_color = Colors::BLACK;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        FxStrobe() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick = tick;
                for (auto* trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        flash_tick = tick;
                    }
                }

                cache_color = Colors::BLACK;

                if (!color_inputs.empty() && flash_tick != UINT32_MAX && tick >= flash_tick &&
                    tick < flash_tick + getParam(0)) {
                    cache_color = color_inputs.at(0)->getColor(tick, this);
                }
            }
            return cache_color;
        }
    };

    constexpr NodeConfig FxStrobe::config =
        NodeConfig("FxStrobe", "Strobe effect", 1, INPUTS_UNLIMITED, true, false, {{"flash_length", 1, 0xFFFF, 1}});
}
