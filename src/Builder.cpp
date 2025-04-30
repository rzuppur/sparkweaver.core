#include <array>
#include <cstring>

#include "Node.h"
#include "SparkWeaverCore.h"
#include "nodes/DsDmxRgb.h"
#include "nodes/FxBreathe.h"
#include "nodes/FxStrobe.h"
#include "nodes/MxAdd.h"
#include "nodes/MxSubtract.h"
#include "nodes/SrColor.h"
#include "nodes/TrCycle.h"
#include "nodes/TrDelay.h"
#include "nodes/TrRandom.h"

namespace SparkWeaverCore {
    namespace {
        const auto node_configs = getNodeConfigs();

        const NodeConfig* getConfig(const std::string_view node_key)
        {
            for (const auto& config : node_configs) {
                if (std::string_view(config.name.data()) == node_key) {
                    return &config;
                }
            }
            return nullptr;
        }

        using NodeCtor = Node* (*)();
        struct NodeFactory {
            std::string_view key;
            NodeCtor         make;
        };

        Node* makeNode(const std::string_view node_key)
        {
            static const std::vector<NodeFactory> node_registry{
                {DsDmxRgb::config.name.data(), []() -> Node* { return new DsDmxRgb(); }},
                {FxBreathe::config.name.data(), []() -> Node* { return new FxBreathe(); }},
                {FxStrobe::config.name.data(), []() -> Node* { return new FxStrobe(); }},
                {MxAdd::config.name.data(), []() -> Node* { return new MxAdd(); }},
                {MxSubtract::config.name.data(), []() -> Node* { return new MxSubtract(); }},
                {SrColor::config.name.data(), []() -> Node* { return new SrColor(); }},
                {TrCycle::config.name.data(), []() -> Node* { return new TrCycle(); }},
                {TrDelay::config.name.data(), []() -> Node* { return new TrDelay(); }},
                {TrRandom::config.name.data(), []() -> Node* { return new TrRandom(); }}};

            for (const auto& [key, make] : node_registry) {
                if (key == node_key) {
                    return make();
                }
            }
            return nullptr;
        }

        [[nodiscard]] std::vector<uint16_t> parseParams(const std::string& name, const std::vector<std::string>& params)
        {
            std::vector<uint16_t> result(params.size());
            try {
                for (std::size_t i = 0; i < params.size(); ++i) {
                    const auto value = std::stoul(params[i]);
                    if (value > UINT16_MAX) {
                        throw InvalidTreeException(name + " parameter outside range");
                    }
                    result[i] = static_cast<uint16_t>(value);
                }
            } catch (...) {
                throw InvalidTreeException(name + " invalid parameters");
            }
            return result;
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

    void Builder::closeParam() noexcept
    {
        if (!buffer_current_param.empty()) {
            buffer_all_params.push_back(buffer_current_param);
            buffer_current_param.clear();
        }
    }

    void Builder::clearCommandBuffer() noexcept
    {
        buffer_command.clear();
        buffer_command_parsed = false;
        buffer_current_param.clear();
        buffer_all_params.clear();
    }

    void Builder::finishCommand()
    {
        closeParam();
        if (!buffer_command.empty()) {
            parseTreeCommand(buffer_command, buffer_all_params);
        }
        clearCommandBuffer();
    }

    void Builder::parseTreeCommand(const std::string& command, const std::vector<std::string>& params)
    {
        // CONNECTION
        if (command == "C" || command == "T") {
            if (params.size() != 2) {
                throw InvalidTreeException(command + " invalid parameters");
            }
            const auto parsed_params = parseParams("Connection", params);
            const auto from          = parsed_params.at(0);
            const auto to            = parsed_params.at(1);
            if (from >= all_nodes.size() || to >= all_nodes.size()) {
                throw InvalidTreeException(
                    "Connection " + std::to_string(from) + "-" + std::to_string(to) + " out of range");
            }
            if (command == "C") {
                Node::connectColor(all_nodes.at(from), all_nodes.at(to));
            } else if (command == "T") {
                Node::connectTrigger(all_nodes.at(from), all_nodes.at(to));
            }
        }

        // NODE
        else if (const auto config = getConfig(command)) {
            if (params.size() != config->params_count) {
                throw InvalidTreeException(command + " invalid parameters");
            }
            const auto parsed_params = parseParams(command, params);
            const auto p_node        = makeNode(command);
            if (!p_node) {
                throw InvalidTreeException(command + " not found");
            }
            for (size_t i = 0; i < parsed_params.size(); i++) {
                p_node->setParam(i, parsed_params[i]);
            }
            all_nodes.push_back(p_node);
            if (!config->enable_color_outputs && !config->enable_trigger_outputs) {
                root_nodes.push_back(p_node);
            }
        } else {
            throw InvalidTreeException("Unknown command: " + command);
        }
    }

    void Builder::build(const std::string& tree)
    {
        time = 0;
        resetNodeTree();
        clearCommandBuffer();

        try {
            for (const char c : tree) {
                if (c == '\n') {
                    finishCommand();
                } else if (c == ' ') {
                    if (buffer_command.empty()) {
                        throw InvalidTreeException();
                    }
                    if (buffer_command_parsed) {
                        closeParam();
                    } else {
                        buffer_command_parsed = true;
                    }
                } else {
                    if (buffer_command_parsed) {
                        buffer_current_param.push_back(c);
                    } else {
                        buffer_command.push_back(c);
                    }
                }
            }
            finishCommand();
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
        time++;
        for (const auto root_node : root_nodes) {
            root_node->render(time, dmx_data);
        }
        return dmx_data;
    }
}
