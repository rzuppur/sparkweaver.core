#pragma once

#include <cstdint>

#include "../src/Node.h"
#include "../src/nodes/DsDmxRgb.h"
#include "../src/nodes/FxBreathe.h"
#include "../src/nodes/FxPulse.h"
#include "../src/nodes/FxStrobe.h"
#include "../src/nodes/MxAdd.h"
#include "../src/nodes/MxSequence.h"
#include "../src/nodes/MxSubtract.h"
#include "../src/nodes/MxSwitch.h"
#include "../src/nodes/SrColor.h"
#include "../src/nodes/TrChance.h"
#include "../src/nodes/TrCycle.h"
#include "../src/nodes/TrDelay.h"
#include "../src/nodes/TrRandom.h"
#include "../src/nodes/TrSequence.h"

namespace SparkWeaverCore {
    inline std::vector<NodeConfig> getNodeConfigs()
    {
        return {
            DsDmxRgb::config,
            FxBreathe::config,
            FxPulse::config,
            FxStrobe::config,
            MxAdd::config,
            MxSequence::config,
            MxSubtract::config,
            MxSwitch::config,
            SrColor::config,
            TrChance::config,
            TrCycle::config,
            TrDelay::config,
            TrRandom::config,
            TrSequence::config,
        };
    }

    class Builder {
        uint32_t           current_tick              = 0;
        uint8_t            dmx_data[DMX_PACKET_SIZE] = {};
        std::vector<Node*> root_nodes;
        std::vector<Node*> all_nodes;
        void               resetNodeTree() noexcept;

    public:
        /** @throws InvalidTreeException if the tree contains errors */
        void build(const std::vector<uint8_t>& tree);

        [[nodiscard]] uint8_t* tick() noexcept;
    };
}
