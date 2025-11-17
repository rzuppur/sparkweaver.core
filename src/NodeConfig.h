#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <utility>

#include "Config.h"
#include "NodeConfigParam.h"

namespace SparkWeaverCore {
    enum class ColorOutputs {
        DISABLED,
        ENABLED,
    };

    enum class TriggerOutputs {
        DISABLED,
        ENABLED,
    };

    struct NodeConfig {
        std::array<char, 24>                          name{};
        std::array<NodeConfigParam, PARAMS_MAX_COUNT> params{};
        const uint8_t                                 type_id;
        const uint8_t                                 params_count;
        const uint8_t                                 color_inputs_max;
        const uint8_t                                 trigger_inputs_max;
        const ColorOutputs                            color_outputs;
        const TriggerOutputs                          trigger_outputs;

        NodeConfig() = delete;

        constexpr NodeConfig(
            const uint8_t                                type_id,
            const std::string_view                       _name,
            const uint8_t                                color_inputs_max,
            const uint8_t                                trigger_inputs_max,
            const ColorOutputs                           color_outputs,
            const TriggerOutputs                         trigger_outputs,
            const std::initializer_list<NodeConfigParam> _params)
            : type_id(type_id)
            , params_count(_params.size())
            , color_inputs_max(color_inputs_max)
            , trigger_inputs_max(trigger_inputs_max)
            , color_outputs(color_outputs)
            , trigger_outputs(trigger_outputs)
        {
            copyStringToArray(_name, name);
            assert(params_count <= PARAMS_MAX_COUNT);
            size_t i = 0;
            for (const auto& param : _params) {
                params[i++] = param;
            }
        }
    };
}
