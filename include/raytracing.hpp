#pragma once

#include "ray.hpp"
#include "scene.hpp"
#include "camera.hpp"

#include <cstdint>
#include <vector>

namespace rt {
    glm::vec3 perSample(rt::ray r, const rt::scene& scene, uint32_t& seed);

    void render(std::vector<uint8_t>& resoultImage, const rt::camera& cam, const rt::scene& scene);

}