#pragma once

#include <cstdint>
#include <string>

namespace rt {
    inline uint32_t bounces = 3;
    inline uint32_t samples = 32;

    const uint32_t resolutionX = 800;
    const uint32_t resolutionY = 600;
    const std::string title = "Ray tracing in one weekend";

    inline float fovY = 60.0f;
    inline float jiggle = 0.001f;

    inline float DOFjiggle = 0.1f;
    inline float DOFfocus = 3.0f;
}