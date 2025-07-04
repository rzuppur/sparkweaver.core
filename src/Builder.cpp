#include <array>
#include <cstring>

#include "Node.h"
#include "SparkWeaverCore.h"

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
                {FxPulse::config.name.data(), []() -> Node* { return new FxPulse(); }},
                {FxStrobe::config.name.data(), []() -> Node* { return new FxStrobe(); }},
                {MxAdd::config.name.data(), []() -> Node* { return new MxAdd(); }},
                {MxSequence::config.name.data(), []() -> Node* { return new MxSequence(); }},
                {MxSubtract::config.name.data(), []() -> Node* { return new MxSubtract(); }},
                {MxSwitch::config.name.data(), []() -> Node* { return new MxSwitch(); }},
                {SrColor::config.name.data(), []() -> Node* { return new SrColor(); }},
                {TrChance::config.name.data(), []() -> Node* { return new TrChance(); }},
                {TrCycle::config.name.data(), []() -> Node* { return new TrCycle(); }},
                {TrDelay::config.name.data(), []() -> Node* { return new TrDelay(); }},
                {TrRandom::config.name.data(), []() -> Node* { return new TrRandom(); }},
                {TrSequence::config.name.data(), []() -> Node* { return new TrSequence(); }}};

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
        if (command == "CI" || command == "CO" || command == "TI" || command == "TO") {
            if (params.size() < 2) {
                throw InvalidTreeException(command + " invalid parameters");
            }
            const auto parsed_params = parseParams("Connection", params);
            const auto source        = parsed_params.at(0);
            if (source >= all_nodes.size()) {
                throw InvalidTreeException("Connection node " + std::to_string(source) + " out of range");
            }
            for (auto connection = parsed_params.begin() + 1; connection != parsed_params.end(); ++connection) {
                if (*connection >= all_nodes.size()) {
                    throw InvalidTreeException("Connection node " + std::to_string(*connection) + " out of range");
                }
                if (command == "CI") {
                    all_nodes.at(source)->addColorInput(all_nodes.at(*connection));
                } else if (command == "CO") {
                    all_nodes.at(source)->addColorOutput(all_nodes.at(*connection));
                } else if (command == "TI") {
                    all_nodes.at(source)->addTriggerInput(all_nodes.at(*connection));
                } else if (command == "TO") {
                    all_nodes.at(source)->addTriggerOutput(all_nodes.at(*connection));
                }
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
        current_tick = 0;
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
        for (const auto root_node : root_nodes) {
            root_node->render(current_tick, dmx_data);
        }
        current_tick++;
        return dmx_data;
    }
}
