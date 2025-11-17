#pragma once

#include <cstdint>
#include <unordered_map>

#include "../src/nodes/DsDmxRgb.h"
#include "../src/nodes/FxBreathe.h"
#include "../src/nodes/FxPulse.h"
#include "../src/nodes/FxStrobe.h"
#include "../src/nodes/MxAdd.h"
#include "../src/nodes/MxAnd.h"
#include "../src/nodes/MxOr.h"
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
    using NodeParams = const std::array<uint16_t, PARAMS_MAX_COUNT>&;
    using NodeCtor   = Node* (*)(NodeParams);

    template <typename T>
    Node* createNode(NodeParams p)
    {
        return new T(p);
    }

    struct NodeInfo {
        const NodeConfig* config;
        const NodeCtor    ctor;

        NodeInfo() = delete;

        NodeInfo(const NodeConfig* config, const NodeCtor ctor)
            : config(config)
            , ctor(ctor)
        {
        }
    };

    template <typename T>
    std::pair<uint8_t, NodeInfo> registerNode()
    {
        return {T::config.type_id, NodeInfo(&T::config, createNode<T>)};
    }

    class InvalidTreeException final : public std::exception {
        std::string message;

    public:
        explicit InvalidTreeException(const size_t pos, const std::string& errorMessage)
            : message("Tree @ " + std::to_string(pos) + ": " + errorMessage)
        {
        }

        explicit InvalidTreeException(const size_t pos)
            : message("Tree @ " + std::to_string(pos) + ": Error")
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };

    /**
     * @class Engine
     * @brief Builds and runs the node tree.
     */
    class Engine {
        static inline const std::unordered_map<uint8_t, NodeInfo> node_registry = {
            registerNode<DsDmxRgb>(),
            registerNode<FxBreathe>(),
            registerNode<FxPulse>(),
            registerNode<FxStrobe>(),
            registerNode<MxAdd>(),
            registerNode<MxAnd>(),
            registerNode<MxOr>(),
            registerNode<MxSequence>(),
            registerNode<MxSubtract>(),
            registerNode<MxSwitch>(),
            registerNode<SrColor>(),
            registerNode<SrTrigger>(),
            registerNode<TrChance>(),
            registerNode<TrCycle>(),
            registerNode<TrDelay>(),
            registerNode<TrRandom>(),
            registerNode<TrSequence>()};

        uint32_t                      current_tick              = 0;
        uint8_t                       dmx_data[DMX_PACKET_SIZE] = {};
        std::vector<NodeLinkColor*>   color_links{};
        std::vector<NodeLinkTrigger*> trigger_links{};
        std::vector<Node*>            root_nodes{};
        std::vector<Node*>            all_nodes{};

        static const NodeConfig* getNodeConfig(uint8_t type_id) noexcept;
        static Node*             makeNode(uint8_t type_id, NodeParams params) noexcept;

        void reset() noexcept;

    public:
        Engine() = default;

        ~Engine();

        /**
         * @brief  Get supported node types.
         * @return Configurations of all nodes
         */
        static std::vector<const NodeConfig*> getNodeConfigs() noexcept;

        /**
         * @brief Resets the node tree and tries to parse a new tree.
         * @param tree Serialized tree bytes
         * @throws InvalidTreeException If the tree contains errors
         * @throws InvalidLinkException If the tree has invalid links
         */
        void build(const std::vector<uint8_t>& tree);

        /**
         * @brief Increment global clock and execute all nodes.
         * @return Pointer to 513 bytes long DMX data output, byte number corresponds to DMX address, 0 is unused
         */
        [[nodiscard]] const uint8_t* tick() noexcept;

        /**
         * @brief Get available external triggers.
         * @return Valid trigger ID-s
         */
        [[nodiscard]] std::vector<uint8_t> listExternalTriggers() const noexcept;

        /**
         * @brief Send external trigger.
         * @note Triggers will activate on next tick since \c current_tick is always one step ahead of the last render.
         * @param id ID of trigger
         */
        void triggerExternalTrigger(uint8_t id) const noexcept;
    };
}
