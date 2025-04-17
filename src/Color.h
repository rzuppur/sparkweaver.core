#pragma once

#include <algorithm>
#include <cstdint>

namespace SparkWeaverCore {
    class Color {
    public:
        uint8_t r, g, b;

        constexpr Color(const uint8_t red, const uint8_t green, const uint8_t blue) :
            r(red),
            g(green),
            b(blue)
        {
        }

        constexpr Color operator+(const Color& other) const
        {
            return {
                static_cast<uint8_t>(r > 0xFF - other.r ? 0xFF : r + other.r),
                static_cast<uint8_t>(g > 0xFF - other.g ? 0xFF : g + other.g),
                static_cast<uint8_t>(b > 0xFF - other.b ? 0xFF : b + other.b)};
        }

        constexpr Color operator-(const Color& other) const
        {
            return {
                static_cast<uint8_t>(r < other.r ? 0x00 : r - other.r),
                static_cast<uint8_t>(g < other.g ? 0x00 : g - other.g),
                static_cast<uint8_t>(b < other.b ? 0x00 : b - other.b)};
        }

        constexpr Color operator*(const Color& other) const
        {
            return {
                static_cast<uint8_t>(std::min(0xFF, static_cast<int>(r) * static_cast<int>(other.r) / 0xFF)),
                static_cast<uint8_t>(std::min(0xFF, static_cast<int>(g) * static_cast<int>(other.g) / 0xFF)),
                static_cast<uint8_t>(std::min(0xFF, static_cast<int>(b) * static_cast<int>(other.b) / 0xFF))};
        }

        constexpr Color operator*(const float factor) const
        {
            return {
                static_cast<uint8_t>(std::min(static_cast<float>(0xFF), static_cast<float>(r) * factor)),
                static_cast<uint8_t>(std::min(static_cast<float>(0xFF), static_cast<float>(g) * factor)),
                static_cast<uint8_t>(std::min(static_cast<float>(0xFF), static_cast<float>(b) * factor))};
        }
    };

    namespace Colors {
        constexpr Color WHITE  = {0xFF, 0xFF, 0xFF};
        constexpr Color BLACK  = {0x00, 0x00, 0x00};
        constexpr Color RED    = {0xFF, 0x00, 0x00};
        constexpr Color GREEN  = {0x00, 0xFF, 0x00};
        constexpr Color BLUE   = {0x00, 0x00, 0xFF};
        constexpr Color PURPLE = {0xFF, 0x00, 0xFF};
        constexpr Color CYAN   = {0x00, 0xFF, 0xFF};
        constexpr Color YELLOW = {0xFF, 0xAA, 0x00};
        constexpr Color ORANGE = {0xFF, 0x44, 0x00};
    }
}
