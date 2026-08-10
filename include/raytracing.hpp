#pragma once

#include "ray.hpp"
#include "scene.hpp"
#include "camera.hpp"

#include <cstdint>
#include <vector>

namespace rt {
    void perSample(glm::vec3& beauty, glm::vec3& albedo, glm::vec3& normal, rt::ray r, const rt::scene& scene, uint32_t& seed);

    void render(std::vector<float>& beautyImage, std::vector<float>& albedoImage, std::vector<float>& normalImage, const rt::camera& cam, const rt::scene& scene);

}