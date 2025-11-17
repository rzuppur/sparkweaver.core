#include "Engine.h"

#include <cstdint>
#include <cstring>
#include <set>

#include <utils/SafeVectorReader.h>

namespace SparkWeaverCore {
    const NodeConfig* Engine::getNodeConfig(const uint8_t type_id) noexcept
    {
        if (const auto pair = node_registry.find(type_id); pair != node_registry.end()) return pair->second.config;
        return nullptr;
    }

    Node* Engine::makeNode(const uint8_t type_id, NodeParams params) noexcept
    {
        if (const auto pair = node_registry.find(type_id); pair != node_registry.end())
            return pair->second.ctor(params);
        return nullptr;
    }

    void Engine::reset() noexcept
    {
        for (const auto color_link : color_links)
            delete color_link;
        color_links.clear();

        for (const auto trigger_link : trigger_links)
            delete trigger_link;
        trigger_links.clear();

        for (const auto all_node : all_nodes)
            delete all_node;
        all_nodes.clear();
        root_nodes.clear();

        current_tick = 0;
    }

    Engine::~Engine() { reset(); }

    std::vector<const NodeConfig*> Engine::getNodeConfigs() noexcept
    {
        std::vector<const NodeConfig*> configs;
        // ReSharper disable once CppUseElementsView
        for (const auto& [first, second] : node_registry) {
            configs.push_back(second.config);
        }
        return configs;
    }

    void Engine::build(const std::vector<uint8_t>& tree)
    {
        reset();

        try {
            SafeVectorReader reader(tree);

            // Check version
            if (!reader.hasByte()) throw InvalidTreeException(0, "Tree is empty");
            if (reader.readByte() != TREE_VERSION)
                throw InvalidTreeException(reader.position(), "Incompatible tree version");

            // Parse commands
            while (reader.hasByte()) {
                if (const auto command = reader.readByte();
                    command == CommandIds::ColorLinks || command == CommandIds::TriggerLinks) {
                    // Get links count
                    if (!reader.hasShort()) throw InvalidTreeException(reader.position(), "Links missing length");
                    const auto count = reader.readShort();

                    // Read links
                    for (auto i = 0; i < count; i++) {
                        if (!reader.hasBytes(6)) throw InvalidTreeException(reader.position(), "Link incomplete");
                        const auto out_node_index = reader.readShort();
                        const auto in_node_index  = reader.readShort();
                        const auto out_index      = reader.readByte();
                        const auto in_index       = reader.readByte();
                        if (out_node_index >= all_nodes.size() || in_node_index >= all_nodes.size())
                            throw InvalidTreeException(reader.position(), "Link index out of range");

                        // Make link
                        const auto p_out = all_nodes[out_node_index];
                        const auto p_in  = all_nodes[in_node_index];
                        if (command == CommandIds::ColorLinks)
                            color_links.emplace_back(new NodeLinkColor(p_out, p_in, out_index, in_index));
                        else
                            trigger_links.emplace_back(new NodeLinkTrigger(p_out, p_in, out_index, in_index));
                    }

                } else {
                    // Find corresponding node config
                    const auto p_config = getNodeConfig(command);
                    if (p_config == nullptr) throw InvalidTreeException(reader.position(), "Unknown command");

                    // Read params
                    std::array<uint16_t, PARAMS_MAX_COUNT> params = {};
                    for (auto i = 0; i < p_config->params_count; i++) {
                        if (!reader.hasShort()) throw InvalidTreeException(reader.position(), "Missing parameter");
                        params[i] = reader.readShort();
                    }

                    // Make node
                    const auto p_node = makeNode(p_config->type_id, params);
                    all_nodes.push_back(p_node);

                    // If node has no outputs add it to root nodes
                    if (p_config->color_outputs == ColorOutputs::DISABLED &&
                        p_config->trigger_outputs == TriggerOutputs::DISABLED)
                        root_nodes.push_back(p_node);
                }
            }

        } catch (...) {
            reset();
            throw;
        }
    }

    [[nodiscard]] const uint8_t* Engine::tick() noexcept
    {
        memset(dmx_data, 0, sizeof(dmx_data));
        for (const auto root_node : root_nodes) {
            root_node->render(current_tick, dmx_data);
        }
        current_tick++;
        return dmx_data;
    }

    std::vector<uint8_t> Engine::listExternalTriggers() const noexcept
    {
        std::set<uint8_t> ids;
        for (const auto& node : all_nodes) {
            if (node->getConfig().type_id == TypeIds::SrTrigger) {
                ids.insert(node->getParam(0));
            }
        }
        return {ids.begin(), ids.end()};
    }

    void Engine::triggerExternalTrigger(const uint8_t id) const noexcept
    {
        for (const auto& node : all_nodes) {
            if (node->getConfig().type_id == TypeIds::SrTrigger) {
                if (node->getParam(0) == id) {
                    node->trigger(current_tick);
                }
            }
        }
    }
}
