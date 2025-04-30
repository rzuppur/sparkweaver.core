#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Color.h"
#include "utils/string.h"

namespace SparkWeaverCore {
    constexpr int DMX_PACKET_SIZE  = 513;
    constexpr int INPUTS_UNLIMITED = 32;
    constexpr int NODE_PARAMS_MAX  = 8;

    struct NodeParam {
        std::array<char, 16> name{};
        uint16_t             min;
        uint16_t             max;
        uint16_t             default_value;

        constexpr NodeParam()
            : min(0)
            , max(0)
            , default_value(0)
        {
        }

        constexpr NodeParam(
            const std::string_view _name,
            const uint16_t         min,
            const uint16_t         max,
            const uint16_t         default_value)
            : min(min)
            , max(max)
            , default_value(default_value)
        {
            copyStringToArray(_name, name);
        }
    };

    struct NodeConfig {
        std::array<char, 24>                   title{};
        std::array<char, 16>                   name{};
        std::array<NodeParam, NODE_PARAMS_MAX> params{};
        const uint8_t                          params_count;
        const uint8_t                          max_color_inputs;
        const uint8_t                          max_trigger_inputs;
        const bool                             enable_color_outputs;
        const bool                             enable_trigger_outputs;

        NodeConfig() = delete;

        constexpr NodeConfig(
            const std::string_view                 _name,
            const std::string_view                 _title,
            const uint8_t                          max_color_inputs,
            const uint8_t                          max_trigger_inputs,
            const bool                             enable_color_outputs,
            const bool                             enable_trigger_outputs,
            const std::initializer_list<NodeParam> _params)
            : params_count(_params.size())
            , max_color_inputs(max_color_inputs)
            , max_trigger_inputs(max_trigger_inputs)
            , enable_color_outputs(enable_color_outputs)
            , enable_trigger_outputs(enable_trigger_outputs)
        {
            copyStringToArray(_name, name);
            copyStringToArray(_title, title);
            assert(params_count <= NODE_PARAMS_MAX);
            auto _params_it = _params.begin();
            for (size_t i = 0; _params_it != _params.end(); ++i) {
                params[i] = *_params_it;
                std::advance(_params_it, 1);
            }
        }
    };

    class Node {
        static inline NodeConfig default_config{"Node", "Empty node", 0, 0, false, false, {}};

        std::array<uint16_t, NODE_PARAMS_MAX> params{};

    protected:
        // COLOR IN
        std::vector<Node*> color_inputs;
        void               addColorInput(Node* input);

        // COLOR OUT
        std::vector<Node*> color_outputs;
        [[nodiscard]] int  getColorOutputIndex(const Node* to) const noexcept;
        void               addColorOutput(Node* output);

        // TRIGGER IN
        std::vector<Node*> trigger_inputs;
        void               addTriggerInput(Node* input);

        // TRIGGER OUT
        std::vector<Node*> trigger_outputs;
        [[nodiscard]] int  getTriggerOutputIndex(const Node* to) const noexcept;
        void               addTriggerOutput(Node* output);

    public:
        virtual ~Node() = default;

        /**
         * @brief Sets up parameters. Should be called in every derived class constructor.
         */
        void init() noexcept
        {
            auto const& config = getConfig();
            for (size_t i = 0; i < config.params_count; i++) {
                params[i] = config.params[i].default_value;
            }
        }

        [[nodiscard]] uint16_t getParam(const size_t n) const noexcept
        {
            if (n >= getConfig().params_count) return 0;
            return params[n];
        }

        void setParam(const size_t n, const uint16_t value) noexcept
        {
            const auto config = getConfig();
            if (n >= config.params_count) return;
            if (value < config.params[n].min || value > config.params[n].max) return;
            params[n] = value;
        }

        /**
         * @brief Should be overridden by derived class to return the correct configuration
         */
        [[nodiscard]] virtual const NodeConfig& getConfig() const noexcept { return default_config; }

        [[nodiscard]] std::string getName() const noexcept { return {getConfig().name.data()}; }

        [[nodiscard]] virtual Color getColor(uint32_t tick, const Node* requested_by) noexcept { return Colors::BLACK; }

        [[nodiscard]] virtual bool getTrigger(uint32_t tick, const Node* requested_by) noexcept { return false; }

        /**
         * @brief Evaluate the node tree and render it to a DMX data array.
         *
         * @param tick Current tick number
         * @param p_dmx_data Pointer to 513 bytes long array corresponding to DMX addresses, first byte is unused
         */
        virtual void render(uint32_t tick, uint8_t* p_dmx_data) noexcept {}

        /** @throws InvalidConnectionException if trying to make an invalid connection */
        static void connectColor(Node* from, Node* to);

        /** @throws InvalidConnectionException if trying to make an invalid connection */
        static void connectTrigger(Node* from, Node* to);
    };

    class InvalidConnectionException final : public std::exception {
        std::string message;

    public:
        InvalidConnectionException(const std::string& nodeName, const std::string& errorMessage)
            : message("Invalid connection to " + nodeName + ": " + errorMessage)
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };

    class InvalidParameterException final : public std::exception {
        std::string message;

    public:
        InvalidParameterException(const std::string& nodeName, const std::string& errorMessage)
            : message("Invalid parameter for " + nodeName + ": " + errorMessage)
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };

    class InvalidTreeException final : public std::exception {
        std::string message;

    public:
        explicit InvalidTreeException(std::string errorMessage)
            : message(std::move(errorMessage))
        {
        }

        InvalidTreeException()
            : message("Invalid node tree structure")
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };
}
