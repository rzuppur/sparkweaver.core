// ReSharper disable CppUseRangeAlgorithm
#include "Node.h"

namespace SparkWeaverCore {
    // COLOR IN

    void Node::addColorInput(Node* const input)
    {
        if (getConfig().max_color_inputs > color_inputs.size()) {
            color_inputs.push_back(input);
        } else {
            throw InvalidConnectionException(getName(), "max_color_inputs exceeded");
        }
    }

    // COLOR OUT

    int Node::getColorOutputIndex(const Node* const to) const noexcept
    {
        if (const auto i = std::find(color_outputs.begin(), color_outputs.end(), to); i != color_outputs.end()) {
            return static_cast<int>(std::distance(color_outputs.begin(), i));
        }
        return -1;
    }

    void Node::addColorOutput(Node* const output)
    {
        if (getConfig().enable_color_outputs) {
            color_outputs.push_back(output);
        } else {
            throw InvalidConnectionException(getName(), "color_outputs disabled");
        }
    }

    // TRIGGER IN

    void Node::addTriggerInput(Node* const input)
    {
        if (getConfig().max_trigger_inputs > trigger_inputs.size()) {
            trigger_inputs.push_back(input);
        } else {
            throw InvalidConnectionException(getName(), "max_trigger_inputs exceeded");
        }
    }

    // TRIGGER OUT

    int Node::getTriggerOutputIndex(const Node* const to) const noexcept
    {
        if (const auto i = std::find(trigger_outputs.begin(), trigger_outputs.end(), to); i != trigger_outputs.end()) {
            return static_cast<int>(std::distance(trigger_outputs.begin(), i));
        }
        return -1;
    }

    void Node::addTriggerOutput(Node* const output)
    {
        if (getConfig().enable_trigger_outputs) {
            trigger_outputs.push_back(output);
        } else {
            throw InvalidConnectionException(getName(), "trigger_outputs disabled");
        }
    }

    // STATIC

    void Node::connectColor(Node* const from, Node* const to)
    {
        from->addColorOutput(to);
        to->addColorInput(from);
    }

    void Node::connectTrigger(Node* const from, Node* const to)
    {
        from->addTriggerOutput(to);
        to->addTriggerInput(from);
    }
}
