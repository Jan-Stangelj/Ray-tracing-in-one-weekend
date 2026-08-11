#include "random.hpp"
#include "glm/ext/quaternion_geometric.hpp"

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
        float x1, x2, s;

        do {
            x1 = 2.0f * randomFloat(seed) - 1.0f;
            x2 = 2.0f * randomFloat(seed) - 1.0f;
            s = x1 * x1 + x2 * x2;
        } while (s >= 1.0f);

        float z = 1.0f - 2.0f * s;
        float t = 2.0f * std::sqrt(1.0f - s);

        return glm::vec3(x1 * t, x2 * t, z);
    }

    glm::vec2 randomVec2InUnitDisk(uint32_t& seed) {
        float x, y, s;

        do {
            x = 2.0f * randomFloat(seed) - 1.0f;
            y = 2.0f * randomFloat(seed) - 1.0f;
            s = x * x + y * y;
        } while (s >= 1.0f);

        return glm::vec2(x, y);
    }

    glm::vec3 randomHemisphereVec3(uint32_t& seed, const glm::vec3& normal) {
        glm::vec3 dir = randomUnitVec3(seed);

        if (glm::dot(normal, dir) <= 0.0f)
            dir = -dir;

        return dir;
    }
}