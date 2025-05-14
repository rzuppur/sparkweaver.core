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

    size_t Node::getColorOutputIndex(const Node* const to) const noexcept
    {
        for (size_t i = 0; i < color_outputs.size(); i++) {
            if (color_outputs[i] == to) {
                return i;
            }
        }
        return 0;
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

    size_t Node::getTriggerOutputIndex(const Node* const to) const noexcept
    {
        for (size_t i = 0; i < trigger_outputs.size(); i++) {
            if (trigger_outputs[i] == to) {
                return i;
            }
        }
        return 0;
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
