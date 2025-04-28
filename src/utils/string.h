#pragma once

#include <array>
#include <string_view>

namespace SparkWeaverCore {
    template <size_t T>
    constexpr void copyStringToArray(std::string_view string, std::array<char, T>& array)
    {
        static_assert(T > 0);
        const auto length = string.size() < T - 1 ? string.size() : T - 1;
        for (std::size_t i = 0; i < length; i++) {
            array[i] = string[i];
        }
        array[T - 1] = '\0';
    }
}
