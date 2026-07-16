#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace rt {
    uint32_t PCGhash(uint32_t input);
    float randomFloat(uint32_t& seed);

    glm::vec3 randomVec3(uint32_t& seed, float min, float max);
    glm::vec3 randomUnitVec3(uint32_t& seed);
    glm::vec3 randomHemisphereVec3(uint32_t& seed, const glm::vec3& normal);
}