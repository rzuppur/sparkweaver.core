#pragma once

#include "../Node.h"

namespace SparkWeaverCore {
    class DsDmxRgb final : public Node {
    public:
        static const NodeConfig config;

        [[nodiscard]] const NodeConfig& getConfig() const noexcept override { return config; }

        DsDmxRgb() { init(); }

        void render(const uint32_t tick, uint8_t* p_dmx_data) noexcept override
        {
            const auto address      = getParam(0);
            auto [red, green, blue] = color_inputs.empty() ? Colors::BLACK : color_inputs.at(0)->getColor(tick, this);
            p_dmx_data[address]     = red;
            p_dmx_data[address + 1] = green;
            p_dmx_data[address + 2] = blue;
        }
    };

    constexpr NodeConfig DsDmxRgb::config =
        NodeConfig("DsDmxRgb", "DMX RGB", 1, 0, false, false, {{"address", 1, 510, 1}});
}
