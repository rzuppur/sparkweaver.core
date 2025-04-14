#pragma once

#include <random>

namespace SparkWeaverCore {
    inline int random(const int from, const int to)
    {
        if (from >= to) return from;
        static std::random_device     random_device;
        static std::mt19937           mersenne_twister_engine(random_device());
        std::uniform_int_distribution distribution(from, to);
        return distribution(mersenne_twister_engine);
    }
}
