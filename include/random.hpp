#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace rt {
    uint32_t PCGhash(uint32_t input) {
        uint32_t state = input * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    float randomFloat(uint32_t& seed) {
        seed = PCGhash(seed);
        return (float)seed / (float)UINT32_MAX;
    }

    glm::vec3 randomVec3(uint32_t& seed, float min, float max) {
        return glm::vec3(min+(max-min)*randomFloat(seed), min+(max-min)*randomFloat(seed), min+(max-min)*randomFloat(seed));
    }

    glm::vec3 randomUnitVec3(uint32_t& seed) {
        float theta = 2.0f * 3.1415926535f * randomFloat(seed);  // azimuth
        float phi = acos(1.0f - 2.0f * randomFloat(seed));       // inclination

        float sinPhi = sin(phi);
        return glm::vec3(cos(theta) * sinPhi, sin(theta) * sinPhi, cos(phi));
    }
}