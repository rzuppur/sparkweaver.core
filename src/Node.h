#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Color.h"
#include "Config.h"
#include "NodeConfig.h"
#include "utils/string.h"

namespace SparkWeaverCore {
    class NodeLinkColor;
    class NodeLinkTrigger;

    /**
     * @class Node
     * @attention Node links should be set by \c NodeLink constructor and not modified later. Node should \c get all its
     * inputs during each tick, even if they are not used. Functions that run after the initial tree build should never
     * throw to avoid crashes in live environment.
     */
    class Node {
        static inline NodeConfig
            default_config{0, "Empty node", 0, 0, ColorOutputs::DISABLED, TriggerOutputs::DISABLED, {}};

        const std::array<uint16_t, PARAMS_MAX_COUNT>
            params{}; // Params are currently const but Nodes should support parameter changes during runtime

    protected:
        explicit Node(const std::array<uint16_t, PARAMS_MAX_COUNT> params)
            : params(params)
        {
        }

    public:
        std::vector<NodeLinkColor*>   color_inputs          = {};
        std::vector<NodeLinkTrigger*> trigger_inputs        = {};
        uint8_t                       color_outputs_count   = 0;
        uint8_t                       trigger_outputs_count = 0;

        virtual ~Node() = default;

        /**
         * @brief Should be overridden by derived class to return the correct configuration.
         * @return Node configuration
         */
        [[nodiscard]] virtual const NodeConfig& getConfig() const noexcept { return default_config; }

        /**
         * @brief Get parameter value, returns 0 if index is invalid.
         * @param n Parameter index
         * @return Parameter current value
         */
        [[nodiscard]] uint16_t getParam(const size_t n) const noexcept
        {
            if (n >= getConfig().params_count) return 0;
            return params[n];
        }

        /**
         * @brief Get color output value.
         * @param tick Current tick
         * @param index Output link index
         * @return Color value
         */
        [[nodiscard]] virtual Color getColor(uint32_t tick, const uint8_t index) noexcept { return Colors::BLACK; }

        /**
         * @brief Get trigger output value.
         * @param tick Current tick
         * @param index Output link index
         * @return Trigger value
         */
        [[nodiscard]] virtual bool getTrigger(uint32_t tick, const uint8_t index) noexcept { return false; }

        /**
         * @brief Trigger node from external source.
         * @param tick Current tick number
         */
        virtual void trigger(uint32_t tick) noexcept {}

        /**
         * @brief Evaluate all node inputs and render node output to a DMX packet.
         * @param tick Current tick number
         * @param p_dmx_data Pointer to 513 bytes long array corresponding to DMX addresses, first byte is unused
         */
        virtual void render(uint32_t tick, uint8_t* p_dmx_data) noexcept {}
    };
}
