#pragma once

#include "../Node.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class MxSwitch
     * @brief On trigger chooses a single color input to be passed through to all color outputs.
     */
    class MxSwitch final : public Node {
        size_t   active_index     = 0;
        uint32_t cache_tick       = UINT32_MAX;
        Color    cache_color      = Colors::BLACK;
        bool     is_first_trigger = true;

    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        MxSwitch() { init(); }

        [[nodiscard]] Color getColor(const uint32_t tick, const Node* requested_by) noexcept override
        {
            if (tick != cache_tick) {
                cache_tick          = tick;
                bool trigger_active = false;
                for (auto* trigger_input : trigger_inputs) {
                    if (trigger_input->getTrigger(tick, this)) {
                        trigger_active = true;
                    }
                }
                if (!color_inputs.empty()) {
                    if (trigger_active) {
                        if (getParam(0) == 1) {
                            active_index = random(0, static_cast<int>(color_inputs.size()) - 1);
                        } else {
                            if (is_first_trigger) {
                                is_first_trigger = false;
                                active_index     = 0;
                            } else {
                                active_index = (active_index + 1) % color_inputs.size();
                            }
                        }
                    }
                    cache_color = color_inputs.at(active_index)->getColor(tick, this);
                }
            }
            return cache_color;
        }
    };

    constexpr NodeConfig MxSwitch::config = NodeConfig(
        TypeIds::MxSwitch,
        "Color switch",
        MAXIMUM_CONNECTIONS,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {{"random", 0, 1, 0}});
}
