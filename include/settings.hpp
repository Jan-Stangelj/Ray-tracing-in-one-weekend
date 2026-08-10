#pragma once

#include <cstdint>
#include <string>

namespace rt {
    constexpr float epsilon = 0.000001f;//std::numeric_limits<float>::epsilon();

    inline uint32_t minBounces = 4;
    inline uint32_t maxBounces = 16;
    inline uint32_t samples = 32;

    inline float airIOR = 1.0f;

    inline float fovY = 80.0f;
    inline float AAjiggle = epsilon;

    inline float DOFjiggle = 0.0f;
    inline float DOFfocus = 1.0f;

    constexpr uint32_t resolutionX = 960;
    constexpr uint32_t resolutionY = 720;
    const std::string title = "Ray tracing in one weekend";
}