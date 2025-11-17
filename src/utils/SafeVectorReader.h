#pragma once

#include <cstdint>
#include <vector>

namespace SparkWeaverCore {
    /**
     * @class SafeVectorReader
     * @brief Returns zero if read is out of bounds, use \c hasByte and \c hasShort to check before reading.
     */
    class SafeVectorReader final {
        const std::vector<uint8_t>& bytes;
        size_t                      head = 0;

    public:
        explicit SafeVectorReader(const std::vector<uint8_t>& bytes)
            : bytes(bytes)
        {
        }

        [[nodiscard]] size_t position() const noexcept { return head; }
        [[nodiscard]] bool   hasByte() const noexcept { return head < bytes.size(); }
        [[nodiscard]] bool   hasShort() const noexcept { return head + 1 < bytes.size(); }
        [[nodiscard]] bool   hasBytes(const size_t count) const noexcept { return head + count - 1 < bytes.size(); }

        [[nodiscard]] uint8_t readByte() noexcept
        {
            if (!hasByte()) return 0;
            return bytes[head++];
        }

        [[nodiscard]] uint16_t readShort() noexcept
        {
            if (!hasShort()) return 0;
            const auto lsb = readByte();
            const auto msb = readByte();
            return static_cast<uint16_t>(msb) << 8 | lsb;
        }
    };
}
