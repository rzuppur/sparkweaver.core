#pragma once

#include <cstdint>

#include "utils/string.h"

namespace SparkWeaverCore {
    struct NodeConfigParam {
        std::array<char, 16> name{};
        uint16_t             min;
        uint16_t             max;
        uint16_t             default_value;

        constexpr NodeConfigParam()
            : min(0)
            , max(0)
            , default_value(0)
        {
        }

        constexpr NodeConfigParam(
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
}
