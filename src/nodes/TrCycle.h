#pragma once

#include "../NodeLink.h"

namespace SparkWeaverCore {
    /**
     * @class TrCycle
     * @brief Outputs a trigger based on configured interval.
     */
    class TrCycle final : public Node {
    public:
        static const NodeConfig config;

        explicit TrCycle(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : Node(params)
        {
        }

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        [[nodiscard]] bool getTrigger(const uint32_t tick, const uint8_t index) noexcept override
        {
            const uint16_t cycle_length = getParam(0);
            const uint16_t phase_offset = getParam(1);
            return (tick + phase_offset) % cycle_length == 0;
        }
    };

    constexpr NodeConfig TrCycle::config = NodeConfig(
        TypeIds::TrCycle,
        "Interval trigger",
        0,
        0,
        ColorOutputs::DISABLED,
        TriggerOutputs::ENABLED,
        {{"cycle_length", 1, PARAM_MAX_VALUE, 40}, {"phase_offset", 0, PARAM_MAX_VALUE, 0}});
}
