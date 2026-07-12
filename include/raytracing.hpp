#pragma once

#include "ray.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "random.hpp"
#include "camera.hpp"

#include <cstdint>

namespace rt {
        rt::hitInfo traceRay(rt::ray r, const rt::scene& scene) {
        rt::hitInfo hit;

        if (!scene.hit(r, hit)) {
            float skyVar = (glm::normalize(r.direction()).y + 1) / 2;

            glm::vec3 skyColur(0.5f, 0.7f, 1.0f);
            glm::vec3 groundColur(1.0f);

            hit.solidAdd = glm::vec3(skyVar * skyColur + (1.0f-skyVar) * groundColur);
        }

        return hit;
    }

    glm::vec3 perSample(rt::ray r, const rt::scene& scene, uint32_t& seed) {
        glm::vec3 resoult(1.0f);

        rt::hitInfo hit;

        for (unsigned int i = 0; i < rt::bounces; i++) {
            hit = traceRay(r, scene);

            if (hit.sphere == UINT32_MAX) {
                return resoult * hit.solidAdd;
            }
            if (scene.spheres.at(hit.sphere).getEmission() != glm::vec3(0.0f)) {
                return resoult * scene.spheres.at(hit.sphere).getEmission();
            }

            resoult *= scene.spheres.at(hit.sphere).getColur();

            glm::vec3 newOrigin = hit.origin + hit.normal * 0.0001f;
            glm::vec3 newDirection = glm::normalize(hit.normal + rt::randomUnitVec3(seed));

            r = rt::ray(newOrigin, newDirection);
        }

        return resoult;
    }

    void render(std::vector<uint8_t>& resoultImage, const rt::camera& cam, const rt::scene& scene) {

        #pragma omp parallel for
        for (unsigned int y = 0; y < cam.resolutionY(); y++) {
            for (unsigned int x = 0; x < cam.resolutionX(); x++) {
                rt::ray ray = cam.genRay(x, y);

                glm::vec3 resoult(0.0f);

                uint32_t seed = rt::PCGhash(x) + rt::PCGhash(y);

                for (unsigned int i = 0; i < rt::samples; i++) {
                    rt::ray r(ray.origin(), glm::normalize(ray.direction() + rt::randomUnitVec3(seed)*rt::jiggle));
                    resoult += perSample(r, scene, seed);
                }

                resoult /= rt::samples;

                resoult = glm::clamp(resoult, 0.0f, 1.0f);

                resoult = glm::pow(resoult, glm::vec3(1.0f/2.2f));

                uint32_t index = (y * rt::resolutionX + x)*3;
                resoultImage.at(index) = resoult.r*255;
                resoultImage.at(index+1) = resoult.g*255;
                resoultImage.at(index+2) = resoult.b*255;
            }
        }
    }
}