#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <Color.h>

namespace SparkWeaverCore {
    constexpr int DMX_PACKET_SIZE  = 513;
    constexpr int INPUTS_UNLIMITED = 32;

    class Node {
    protected:
        std::string name;

        // COLOR IN
        uint8_t            max_color_inputs;
        std::vector<Node*> color_inputs;
        void               addColorInput(Node* input);

        // COLOR OUT
        bool               enable_color_outputs;
        std::vector<Node*> color_outputs;
        [[nodiscard]] int  getColorOutputIndex(const Node* to) const noexcept;
        void               addColorOutput(Node* output);

        // TRIGGER IN
        uint8_t            max_trigger_inputs;
        std::vector<Node*> trigger_inputs;
        void               addTriggerInput(Node* input);

        // TRIGGER OUT
        bool               enable_trigger_outputs;
        std::vector<Node*> trigger_outputs;
        [[nodiscard]] int  getTriggerOutputIndex(const Node* to) const noexcept;
        void               addTriggerOutput(Node* output);

    public:
        Node() = delete;

        Node(const std::string_view name, const uint8_t max_color_inputs, const bool enable_color_outputs,
             const uint8_t max_trigger_inputs, const bool enable_trigger_outputs) :
            name(name),
            max_color_inputs(max_color_inputs),
            enable_color_outputs(enable_color_outputs),
            max_trigger_inputs(max_trigger_inputs),
            enable_trigger_outputs(enable_trigger_outputs)
        {
        }

        virtual ~Node() = default;

        [[nodiscard]] virtual Color getColor(uint32_t tick, const Node* requested_by) noexcept { return Colors::BLACK; }

        [[nodiscard]] virtual bool getTrigger(uint32_t tick, const Node* requested_by) noexcept { return false; }

        /**
         * @brief Evaluate node tree and render it to a DMX data array.
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
        explicit InvalidConnectionException(const std::string& nodeName, const std::string& errorMessage) :
            message("Invalid connection to " + nodeName + ": " + errorMessage)
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };

    class InvalidParameterException final : public std::exception {
        std::string message;

    public:
        explicit InvalidParameterException(const std::string& nodeName, const std::string& errorMessage) :
            message("Invalid parameter for " + nodeName + ": " + errorMessage)
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };

    class InvalidTreeException final : public std::exception {
        std::string message;

    public:
        explicit InvalidTreeException(std::string errorMessage) :
            message(std::move(errorMessage))
        {
        }

        InvalidTreeException() :
            message("Invalid node tree structure")
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };
}
