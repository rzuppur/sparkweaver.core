#pragma once

#include "../NodeLink.h"
#include "../utils/random.h"

namespace SparkWeaverCore {
    /**
     * @class TrSequence
     * @brief Sends incoming triggers to a single trigger output, either sequentially or randomly.
     */
    class TrSequence final : public Node {
        uint8_t  active_index = UINT8_MAX;
        uint32_t last_tick    = UINT32_MAX;
        bool     last_value   = false;

    public:
        static const NodeConfig config;

        explicit TrSequence(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            const auto    output_random = getParam(0) == 1;
            const uint8_t index_max = trigger_outputs_count - 1; // count > 0, otherwise getTrigger wouldn't be called

            if (tick != last_tick) {
                last_tick  = tick;
                last_value = false;
                for (auto* trigger_input : trigger_inputs) {
                    last_value = trigger_input->get(tick) || last_value;
                }
                if (last_value) {
                    if (output_random) {
                        active_index = random(0, index_max);
                    } else {
                        active_index = active_index == UINT8_MAX ? 0 : (active_index + 1) % trigger_outputs_count;
                    }
                }
            }

            return last_value && index == active_index;
        }
    };

    constexpr NodeConfig TrSequence::config = NodeConfig(
        TypeIds::TrSequence,
        "Trigger sequence",
        0,
        MAXIMUM_CONNECTIONS,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"random", 0, 1, 0}});
}
