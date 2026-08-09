#pragma once

#include <cstdint>
#include <string>

namespace rt {
    inline uint32_t minBounces = 2;
    inline uint32_t maxBounces = 16;
    inline uint32_t samples = 128;

    inline float airIOR = 1.0f;

    inline float fovY = 60.0f;
    inline float AAjiggle = 0.005f;

    inline float DOFjiggle = 0.0f;
    inline float DOFfocus = 1.0f;

    constexpr uint32_t resolutionX = 800;
    constexpr uint32_t resolutionY = 600;
    const std::string title = "Ray tracing in one weekend";
}