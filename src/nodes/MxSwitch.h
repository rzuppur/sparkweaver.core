#pragma once

#include "../NodeLink.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class MxSwitch
     * @brief On trigger chooses a single color input to be passed through to all color outputs.
     */
    class MxSwitch final : public Node {
        uint8_t  active_index = 0;
        uint32_t last_tick    = UINT32_MAX;

    public:
        static const NodeConfig config;

        explicit MxSwitch(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto    input_random = getParam(0) == 1;
            const uint8_t index_max    = color_inputs.empty() ? 0 : color_inputs.size() - 1;

            if (tick != last_tick) {
                last_tick    = tick;
                auto trigger = false;
                for (auto* trigger_input : trigger_inputs) {
                    trigger = trigger_input->get(tick) || trigger;
                }
                if (trigger) {
                    if (input_random) {
                        active_index = random(0, index_max);
                    } else {
                        active_index = (active_index + 1) % color_inputs.size();
                    }
                }
            }

            if (color_inputs.empty()) return Colors::BLACK;
            return color_inputs.at(active_index)->get(tick);
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
