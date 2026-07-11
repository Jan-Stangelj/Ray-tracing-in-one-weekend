#pragma once

#include <cstdint>
#include <string>

namespace rt {
    uint32_t bounces = 6;
    uint32_t samples = 64;

    uint32_t resolutionX = 800;
    uint32_t resolutionY = 600;
    std::string title = "Ray tracing in one weekend";

    float fovY = 60.0f;
    float jiggle = 0.001f;
}