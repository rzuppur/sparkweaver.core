#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    /**
     * @class DsDmxRgb
     * @brief Sequentially outputs RGB colors to DMX channels starting at a given address.
     */
    class DsDmxRgb final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        DsDmxRgb() { init(); }

        void render(const uint32_t tick, uint8_t* p_dmx_data) noexcept override
        {
            auto address = getParam(0);
            for (auto* color_input : color_inputs) {
                const auto [red, green, blue] = color_input->getColor(tick, this);
                if (address < DMX_PACKET_SIZE) p_dmx_data[address] = red;
                if (address + 1 < DMX_PACKET_SIZE) p_dmx_data[address + 1] = green;
                if (address + 2 < DMX_PACKET_SIZE) p_dmx_data[address + 2] = blue;
                address += 3;
            }
        }
    };

    constexpr NodeConfig DsDmxRgb::config = NodeConfig(
        TypeIds::DsDmxRgb,
        "DMX RGB",
        MAXIMUM_CONNECTIONS,
        0,
        ColorOutputs::DISABLED,
        TriggerOutputs::DISABLED,
        {{"address", 1, 512, 1}});
}
