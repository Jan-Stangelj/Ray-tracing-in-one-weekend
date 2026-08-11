#pragma once

#include <cstdint>
#include <string_view>

namespace rt {
    constexpr float epsilon = 1e-4f;

    inline uint32_t minBounces = 4;
    inline uint32_t maxBounces = 64;
    inline uint32_t samples = 8;

    inline bool denoise = true;
    constexpr bool prefilterAlbedo  = true;

    inline float airIOR = 1.0f;

    inline float fovY = 80.0f;
    inline float AAjiggle = epsilon;

    inline float DOFjiggle = 0.0f;
    inline float DOFfocus = 4.0f;

    constexpr uint32_t resolutionX = 960;
    constexpr uint32_t resolutionY = 720;
    constexpr std::string_view title = "Ray tracing in one weekend";
}