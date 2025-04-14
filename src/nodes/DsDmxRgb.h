#pragma once

#include <Node.h>

namespace SparkWeaverCore {
    class DsDmxRgb final : public Node {
        uint16_t address;

    public:
        explicit DsDmxRgb(const uint16_t address) :
            Node("DsDmxRgb", 1, true, 0, false),
            address(address)
        {
            if (address + 2 >= DMX_PACKET_SIZE) {
                throw InvalidParameterException(name, "DMX address out of range");
            }
        }

        void render(const uint32_t tick, uint8_t* p_dmx_data) noexcept override
        {
            auto [red, green, blue] = color_inputs.empty() ? Colors::BLACK : color_inputs.at(0)->getColor(tick, this);
            p_dmx_data[address]     = red;
            p_dmx_data[address + 1] = green;
            p_dmx_data[address + 2] = blue;
        }
    };
}
