#include <cstring>

#include "Node.h"
#include "SparkWeaverCore.h"

namespace SparkWeaverCore {
    namespace {
        const auto node_configs = getNodeConfigs();

        const NodeConfig* getConfig(const uint8_t type_id)
        {
            for (const auto& config : node_configs) {
                if (config.type_id == type_id) {
                    return &config;
                }
            }
            return nullptr;
        }

        using NodeCtor = Node* (*)();
        struct NodeFactory {
            uint8_t  type_id;
            NodeCtor make;
        };

        Node* makeNode(const uint8_t type_id)
        {
            static const std::vector<NodeFactory> node_registry{
                {DsDmxRgb::config.type_id, []() -> Node* { return new DsDmxRgb(); }},
                {FxBreathe::config.type_id, []() -> Node* { return new FxBreathe(); }},
                {FxPulse::config.type_id, []() -> Node* { return new FxPulse(); }},
                {FxStrobe::config.type_id, []() -> Node* { return new FxStrobe(); }},
                {MxAdd::config.type_id, []() -> Node* { return new MxAdd(); }},
                {MxSequence::config.type_id, []() -> Node* { return new MxSequence(); }},
                {MxSubtract::config.type_id, []() -> Node* { return new MxSubtract(); }},
                {MxSwitch::config.type_id, []() -> Node* { return new MxSwitch(); }},
                {SrColor::config.type_id, []() -> Node* { return new SrColor(); }},
                {TrChance::config.type_id, []() -> Node* { return new TrChance(); }},
                {TrCycle::config.type_id, []() -> Node* { return new TrCycle(); }},
                {TrDelay::config.type_id, []() -> Node* { return new TrDelay(); }},
                {TrRandom::config.type_id, []() -> Node* { return new TrRandom(); }},
                {TrSequence::config.type_id, []() -> Node* { return new TrSequence(); }}};

            for (const auto& [factory_type_id, factory_make] : node_registry) {
                if (factory_type_id == type_id) {
                    return factory_make();
                }
            }
            return nullptr;
        }
    }

    void Builder::resetNodeTree() noexcept
    {
        for (const auto all_node : all_nodes) {
            delete all_node;
        }
        all_nodes.clear();
        root_nodes.clear();
    }

    void Builder::build(const std::vector<uint8_t>& tree)
    {
        current_tick = 0;
        resetNodeTree();

        try {
            int                   chr_pos     = 0;
            uint8_t               command     = 0;
            uint8_t               params_left = 0;
            uint8_t               param_lsb   = 0;
            std::vector<uint16_t> params      = {};

            for (const auto byte : tree) {
                // CHECK VERSION
                if (chr_pos == 0) {
                    if (byte != TREE_VERSION) throw InvalidTreeException(chr_pos, "Incompatible tree version");
                }

                // PARSE PARAMS
                else if (params_left > 0) {
                    if (params_left % 2 == 0) {
                        param_lsb = byte;
                    } else {
                        params.push_back(static_cast<uint16_t>(byte) << 8 | param_lsb);
                    }
                    --params_left;
                    if (params_left == 0) {
                        // CONNECTION
                        if (command == CommandIds::ColorInput || command == CommandIds::TriggerInput ||
                            command == CommandIds::ColorOutput || command == CommandIds::TriggerOutput) {
                            const auto from = params.at(0);
                            const auto to   = params.at(1);
                            if (from >= all_nodes.size() || to >= all_nodes.size() || from == to) {
                                throw InvalidTreeException(chr_pos, "Invalid connection");
                            }
                            if (command == CommandIds::ColorInput) {
                                all_nodes.at(from)->addColorInput(all_nodes.at(to));
                            } else if (command == CommandIds::ColorOutput) {
                                all_nodes.at(from)->addColorOutput(all_nodes.at(to));
                            } else if (command == CommandIds::TriggerInput) {
                                all_nodes.at(from)->addTriggerInput(all_nodes.at(to));
                            } else if (command == CommandIds::TriggerOutput) {
                                all_nodes.at(from)->addTriggerOutput(all_nodes.at(to));
                            }
                        }

                        // NODE
                        else {
                            const auto p_node = makeNode(command);
                            const auto config = getConfig(command);
                            if (!p_node || !config) throw InvalidTreeException(chr_pos);
                            for (size_t i = 0; i < params.size(); i++) {
                                p_node->setParam(i, params[i]);
                            }
                            all_nodes.push_back(p_node);
                            if (!config->enable_color_outputs && !config->enable_trigger_outputs) {
                                root_nodes.push_back(p_node);
                            }
                        }
                    }
                }

                // PARSE COMMAND
                else {
                    params.clear();
                    command = byte;

                    // CONNECTION
                    if (command == CommandIds::ColorInput || command == CommandIds::TriggerInput ||
                        command == CommandIds::ColorOutput || command == CommandIds::TriggerOutput) {
                        params_left = 2 * 2;
                    }

                    // NODE
                    else if (const auto config = getConfig(command)) {
                        if (config->params_count > 0) {
                            params_left = config->params_count * 2;
                        } else {
                            const auto p_node = makeNode(command);
                            if (!p_node) throw InvalidTreeException(chr_pos);
                            all_nodes.push_back(p_node);
                            if (!config->enable_color_outputs && !config->enable_trigger_outputs) {
                                root_nodes.push_back(p_node);
                            }
                        }
                    }

                    // INVALID
                    else
                        throw InvalidTreeException(chr_pos, "Invalid command");
                }

                ++chr_pos;
            }
        } catch (...) {
            resetNodeTree();
            throw;
        }
    }

    /**
     * @brief Increment global clock and execute all nodes
     * @return Pointer to 513 bytes long DMX data array output, byte number corresponds to DMX address, 0 is unused
     */
    [[nodiscard]] uint8_t* Builder::tick() noexcept
    {
        memset(dmx_data, 0, sizeof(dmx_data));
        for (const auto root_node : root_nodes) {
            root_node->render(current_tick, dmx_data);
        }
        current_tick++;
        return dmx_data;
    }
}
