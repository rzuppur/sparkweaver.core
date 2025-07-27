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
    constexpr int     DMX_PACKET_SIZE  = 513;
    constexpr int     INPUTS_UNLIMITED = 32;
    constexpr int     NODE_PARAMS_MAX  = 3;
    constexpr uint8_t TREE_VERSION     = 0x01;

    namespace TypeIds {
        constexpr uint8_t DsDmxRgb   = 0x01;
        constexpr uint8_t FxBreathe  = 0x02;
        constexpr uint8_t FxPulse    = 0x03;
        constexpr uint8_t FxStrobe   = 0x04;
        constexpr uint8_t MxAdd      = 0x05;
        constexpr uint8_t MxSequence = 0x06;
        constexpr uint8_t MxSubtract = 0x07;
        constexpr uint8_t MxSwitch   = 0x08;
        constexpr uint8_t SrColor    = 0x09;
        constexpr uint8_t TrChance   = 0x0A;
        constexpr uint8_t TrCycle    = 0x0B;
        constexpr uint8_t TrDelay    = 0x0C;
        constexpr uint8_t TrRandom   = 0x0D;
        constexpr uint8_t TrSequence = 0x0E;
    }

    namespace CommandIds {
        constexpr uint8_t ColorInput    = 0xFC;
        constexpr uint8_t TriggerInput  = 0xFD;
        constexpr uint8_t ColorOutput   = 0xFE;
        constexpr uint8_t TriggerOutput = 0xFF;
    }

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
        std::array<char, 24>                   name{};
        std::array<NodeParam, NODE_PARAMS_MAX> params{};
        const uint8_t                          type_id;
        const uint8_t                          params_count;
        const uint8_t                          max_color_inputs;
        const uint8_t                          max_trigger_inputs;
        const bool                             enable_color_outputs;
        const bool                             enable_trigger_outputs;

        NodeConfig() = delete;

        constexpr NodeConfig(
            const uint8_t                          type_id,
            const std::string_view                 _name,
            const uint8_t                          max_color_inputs,
            const uint8_t                          max_trigger_inputs,
            const bool                             enable_color_outputs,
            const bool                             enable_trigger_outputs,
            const std::initializer_list<NodeParam> _params)
            : type_id(type_id)
            , params_count(_params.size())
            , max_color_inputs(max_color_inputs)
            , max_trigger_inputs(max_trigger_inputs)
            , enable_color_outputs(enable_color_outputs)
            , enable_trigger_outputs(enable_trigger_outputs)
        {
            copyStringToArray(_name, name);
            assert(params_count <= NODE_PARAMS_MAX);
            auto _params_it = _params.begin();
            for (size_t i = 0; _params_it != _params.end(); ++i) {
                params[i] = *_params_it;
                std::advance(_params_it, 1);
            }
        }
    };

    class Node {
        static inline NodeConfig default_config{0, "Empty node", 0, 0, false, false, {}};

        std::array<uint16_t, NODE_PARAMS_MAX> params{};

    protected:
        // COLOR IN
        std::vector<Node*> color_inputs;

        // COLOR OUT
        std::vector<Node*>   color_outputs;
        [[nodiscard]] size_t getColorOutputIndex(const Node* to) const noexcept;

        // TRIGGER IN
        std::vector<Node*> trigger_inputs;

        // TRIGGER OUT
        std::vector<Node*>   trigger_outputs;
        [[nodiscard]] size_t getTriggerOutputIndex(const Node* to) const noexcept;

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

        [[nodiscard]] uint8_t getTypeId() const noexcept { return {getConfig().type_id}; }

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

        /** @throws InvalidConnectionException */
        void addColorInput(Node* input);

        /** @throws InvalidConnectionException */
        void addColorOutput(Node* output);

        /** @throws InvalidConnectionException */
        void addTriggerInput(Node* input);

        /** @throws InvalidConnectionException */
        void addTriggerOutput(Node* output);
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
        explicit InvalidTreeException(const int chr_pos, const std::string& errorMessage)
            : message("Tree @ " + std::to_string(chr_pos) + ": " + errorMessage)
        {
        }

        InvalidTreeException(const int chr_pos)
            : message("Tree @ " + std::to_string(chr_pos) + ": Error")
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };
}
