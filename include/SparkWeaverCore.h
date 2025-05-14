#pragma once

#include <cstdint>
#include <string>

#include "../src/Node.h"
#include "../src/nodes/DsDmxRgb.h"
#include "../src/nodes/FxBreathe.h"
#include "../src/nodes/FxPulse.h"
#include "../src/nodes/FxStrobe.h"
#include "../src/nodes/MxAdd.h"
#include "../src/nodes/MxSubtract.h"
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
            MxSubtract::config,
            SrColor::config,
            TrChance::config,
            TrCycle::config,
            TrDelay::config,
            TrRandom::config,
            TrSequence::config,
        };
    }

    class Builder {
        uint32_t           time                      = 0;
        uint8_t            dmx_data[DMX_PACKET_SIZE] = {};
        std::vector<Node*> root_nodes;
        std::vector<Node*> all_nodes;
        void               resetNodeTree() noexcept;

        // NODE TREE PARSING
        std::string              buffer_command;
        bool                     buffer_command_parsed = false;
        std::string              buffer_current_param;
        std::vector<std::string> buffer_all_params;
        void                     closeParam() noexcept;
        void                     clearCommandBuffer() noexcept;
        void                     finishCommand();
        void                     parseTreeCommand(const std::string& command, const std::vector<std::string>& params);

    public:
        /** @throws InvalidTreeException if the tree contains errors */
        void build(const std::string& tree);

        [[nodiscard]] uint8_t* tick() noexcept;
    };
}
