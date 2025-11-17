#pragma once

#include <cstdint>

namespace SparkWeaverCore {
    constexpr int      PARAMS_MAX_COUNT    = 4;
    constexpr uint16_t PARAM_MAX_VALUE     = UINT16_MAX;
    constexpr int      DMX_PACKET_SIZE     = 513;
    constexpr int      MAXIMUM_CONNECTIONS = 32;
    constexpr uint8_t  TREE_VERSION        = 0x03;

    namespace TypeIds {
        constexpr uint8_t DsDmxRgb = 0x00;

        constexpr uint8_t FxBreathe = 0x20;
        constexpr uint8_t FxPulse   = 0x21;
        constexpr uint8_t FxStrobe  = 0x22;

        constexpr uint8_t MxAdd      = 0x40;
        constexpr uint8_t MxSequence = 0x41;
        constexpr uint8_t MxSubtract = 0x42;
        constexpr uint8_t MxSwitch   = 0x43;
        constexpr uint8_t MxAnd      = 0x44;
        constexpr uint8_t MxOr       = 0x45;

        constexpr uint8_t SrColor   = 0x60;
        constexpr uint8_t SrTrigger = 0x61;

        constexpr uint8_t TrChance   = 0x80;
        constexpr uint8_t TrCycle    = 0x81;
        constexpr uint8_t TrDelay    = 0x82;
        constexpr uint8_t TrRandom   = 0x83;
        constexpr uint8_t TrSequence = 0x84;
    }

    namespace CommandIds {
        constexpr uint8_t ColorLinks   = 0xFE;
        constexpr uint8_t TriggerLinks = 0xFF;
    }
}
