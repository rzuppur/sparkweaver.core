#pragma once

#include <cstdint>
#include <string>

#include "Node.h"

namespace SparkWeaverCore {
    class InvalidLinkException final : public std::exception {
        const std::string message;

    public:
        explicit InvalidLinkException(const std::string_view message)
            : message(message)
        {
        }

        [[nodiscard]] const char* what() const noexcept override { return message.c_str(); }
    };

    class NodeLinkColor final {
        Node* const   output;
        Node* const   input;
        const uint8_t output_index;
        const uint8_t input_index;
        uint32_t      cache_tick  = UINT32_MAX;
        Color         cache_value = Colors::BLACK;

    public:
        NodeLinkColor(Node* const output, Node* const input, const uint8_t output_index, const uint8_t input_index)
            : output(output)
            , input(input)
            , output_index(output_index)
            , input_index(input_index)
        {
            if (output->getConfig().color_outputs == ColorOutputs::DISABLED)
                throw InvalidLinkException(
                    std::string("Color output not allowed from ") + output->getConfig().name.data());

            if (output->color_outputs_count >= MAXIMUM_CONNECTIONS)
                throw InvalidLinkException(
                    std::string("Maximum color outputs exceeded from ") + output->getConfig().name.data());

            if (input->color_inputs.size() >= input->getConfig().color_inputs_max)
                throw InvalidLinkException(
                    std::string("Maximum color inputs exceeded to ") + input->getConfig().name.data());

            output->color_outputs_count += 1;
            if (input->color_inputs.size() <= input_index) input->color_inputs.resize(input_index + 1);
            input->color_inputs[input_index] = this;
        }

        [[nodiscard]] Color get(const uint32_t tick) noexcept
        {
            if (tick == cache_tick) return cache_value;
            cache_tick  = tick;
            cache_value = output->getColor(tick, output_index);
            return cache_value;
        }
    };

    class NodeLinkTrigger final {
        Node* const   output;
        Node* const   input;
        const uint8_t output_index;
        const uint8_t input_index;
        uint32_t      cache_tick  = UINT32_MAX;
        bool          cache_value = false;

    public:
        NodeLinkTrigger(Node* const output, Node* const input, const uint8_t output_index, const uint8_t input_index)
            : output(output)
            , input(input)
            , output_index(output_index)
            , input_index(input_index)
        {
            if (output->getConfig().trigger_outputs == TriggerOutputs::DISABLED)
                throw InvalidLinkException(
                    std::string("Trigger output not allowed from ") + output->getConfig().name.data());

            if (output->trigger_outputs_count >= MAXIMUM_CONNECTIONS)
                throw InvalidLinkException(
                    std::string("Maximum trigger outputs exceeded from ") + output->getConfig().name.data());

            if (input->trigger_inputs.size() >= input->getConfig().trigger_inputs_max)
                throw InvalidLinkException(
                    std::string("Maximum trigger inputs exceeded to ") + input->getConfig().name.data());

            output->trigger_outputs_count += 1;
            if (input->trigger_inputs.size() <= input_index) input->trigger_inputs.resize(input_index + 1);
            input->trigger_inputs[input_index] = this;
        }

        [[nodiscard]] bool get(const uint32_t tick) noexcept
        {
            if (tick == cache_tick) return cache_value;
            cache_tick  = tick;
            cache_value = output->getTrigger(tick, output_index);
            return cache_value;
        }
    };
}
