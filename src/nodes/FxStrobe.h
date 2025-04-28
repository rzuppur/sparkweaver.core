#pragma once

#include <cstdint>

#include <Node.h>

namespace SparkWeaverCore {
    class FxStrobe final : public Node {
        const uint16_t flash_length;

        uint32_t flash_tick  = UINT32_MAX;
        uint32_t cache_tick  = UINT32_MAX;
        Color    cache_color = Colors::BLACK;

        [[nodiscard]] bool flashActive(const uint32_t tick) const
        {
            return tick >= flash_tick && tick < flash_tick + flash_length && flash_tick != UINT32_MAX;
        }

    public:
        explicit FxStrobe(const uint16_t flash_length) :
            Node(NodeConfig("FxStrobe", "Strobe effect", 1, INPUTS_UNLIMITED, true, false)),
            flash_length(flash_length)
        {
        }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick = tick;
                for (auto* trigger : trigger_inputs) {
                    if (trigger->getTrigger(tick, this)) {
                        flash_tick = tick;
                    }
                }
                if (flashActive(tick) && !color_inputs.empty()) {
                    cache_color = color_inputs.at(0)->getColor(tick, this);
                } else {
                    cache_color = Colors::BLACK;
                }
            }
            return cache_color;
        }
    };
}
