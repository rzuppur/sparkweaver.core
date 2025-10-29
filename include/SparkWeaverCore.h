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
#include "../src/nodes/SrTrigger.h"
#include "../src/nodes/TrChance.h"
#include "../src/nodes/TrCycle.h"
#include "../src/nodes/TrDelay.h"
#include "../src/nodes/TrRandom.h"
#include "../src/nodes/TrSequence.h"

namespace SparkWeaverCore {
    /**
     * @brief List all nodes supported by the Engine
     * @return Configurations of all supported nodes
     */
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
            SrTrigger::config,
            TrChance::config,
            TrCycle::config,
            TrDelay::config,
            TrRandom::config,
            TrSequence::config,
        };
    }

    /**
     * @class Engine
     * @brief Builds and runs the node tree
     */
    class Engine {
        uint32_t           current_tick              = 0;
        uint8_t            dmx_data[DMX_PACKET_SIZE] = {};
        std::vector<Node*> root_nodes;
        std::vector<Node*> all_nodes;
        void               resetNodeTree() noexcept;

    public:
        /**
         * @brief Resets the node tree and tries to parse a new tree
         * @param tree Serialized tree bytes
         * @throws InvalidTreeException if the tree contains errors
         */
        void build(const std::vector<uint8_t>& tree);

        /**
         * @brief Increment global clock and execute all nodes
         * @return Pointer to 513 bytes long DMX data output, byte number corresponds to DMX address, 0 is unused
         */
        [[nodiscard]] uint8_t* tick() noexcept;

        /**
         * @brief Get available external triggers
         * @return Vector with valid trigger ID-s
         */
        [[nodiscard]] std::vector<uint8_t> listExternalTriggers() const noexcept;

        /**
         * @brief Send external trigger
         * @param id ID of trigger
         */
        void                               triggerExternalTrigger(uint8_t id) const noexcept;
    };
}
