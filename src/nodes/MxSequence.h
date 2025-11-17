#pragma once

#include "../NodeLink.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class MxSequence
     * @brief Sends input color to a single color output, changes output on a trigger.
     */
    class MxSequence final : public Node {
        uint8_t  active_index = 0;
        uint32_t last_tick    = UINT32_MAX;

    public:
        static const NodeConfig config;

        explicit MxSequence(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] Color getColor(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto    output_random = getParam(0) == 1;
            const uint8_t index_max     = color_outputs_count - 1; // count > 0, otherwise getColor wouldn't be called

            if (tick != last_tick) {
                last_tick    = tick;
                auto trigger = false;
                for (auto* trigger_input : trigger_inputs) {
                    trigger = trigger_input->get(tick) || trigger;
                }
                if (trigger) {
                    if (output_random) {
                        active_index = random(0, index_max);
                    } else {
                        active_index = (active_index + 1) % color_outputs_count;
                    }
                }
            }

            if (color_inputs.empty()) return Colors::BLACK;
            const auto color = color_inputs.at(0)->get(tick);
            if (index == active_index) return color;
            return Colors::BLACK;
        }
    };

    constexpr NodeConfig MxSequence::config = NodeConfig(
        TypeIds::MxSequence,
        "Color sequence",
        1,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::ENABLED,
        TriggerOutputs::DISABLED,
        {{"random", 0, 1, 0}});
}
