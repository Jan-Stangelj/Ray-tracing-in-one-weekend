#include "raytracing.hpp"

#include "glm/geometric.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "random.hpp"

#include <algorithm>
#include <atomic>
#include <iostream>

glm::vec3 PBRNeutralToneMapping( glm::vec3 color );

namespace rt {

    glm::vec3 perSample(rt::ray r, const rt::scene& scene, uint32_t& seed) {

        glm::vec3 incomingLight(0.0f);
        glm::vec3 rayColur(1.0f);

        for (unsigned int i = 0; i < rt::maxBounces; i++) {
            rt::hitInfo hit;
            
            if (scene.hit(r, hit)) {
                // Hit shader

                const rt::material& material = scene.materials.at(hit.material);

                incomingLight += material.emitted() * rayColur;

                glm::vec3 attenuation(0.0f);
                if (!material.scatter(r, hit, attenuation, seed))
                    break;

                rayColur *= attenuation;
            }
            else {
                // Miss shader
                
                incomingLight += scene.skybox.colorAt(r.direction()) * rayColur;
                break;
            }

            // Russian roulette
            if (i >= minBounces) {
                float luminance = 0.2126*rayColur.r + 0.7152*rayColur.g + 0.0722*rayColur.b;
                luminance = std::clamp(luminance, 0.05f, 1.0f);

                if (randomFloat(seed) > luminance)
                    break;

                rayColur /= luminance;
            }
        }

        return incomingLight;
    }

    void render(std::vector<uint8_t>& resoultImage, const rt::camera& cam, const rt::scene& scene) {
        // Renders all the pixels in parallel
        #pragma omp parallel for
        for (unsigned int y = 0; y < rt::resolutionY; y++) {

            for (unsigned int x = 0; x < rt::resolutionX; x++) {

                rt::ray ray = cam.genRay(x, y);

                glm::vec3 resoult(0.0f);

                uint32_t seed = rt::PCGhash(x * y + x + y);

                for (unsigned int i = 0; i < rt::samples; i++) {
                    glm::vec3 rand = randomUnitVec3(seed);

                    glm::vec3 AAJiggle = (cam.getRight() * rand.x + cam.getUp() * rand.y) * rt::AAjiggle;

                    glm::vec3 DOFjiggle = (cam.getRight() * rand.x + cam.getUp() * rand.y) * rt::DOFjiggle;
                    glm::vec3 DOForigin = ray.origin() + DOFjiggle;
                    glm::vec3 DOFdirection = glm::normalize(ray.at(rt::DOFfocus) - DOForigin);

                    rt::ray r(DOForigin + AAJiggle, DOFdirection);

                    resoult += perSample(r, scene, seed);
                }

                resoult /= rt::samples;

                resoult = PBRNeutralToneMapping(resoult);

                resoult = glm::pow(resoult, glm::vec3(1.0f/2.2f));

                uint32_t index = (y * rt::resolutionX + x)*3;
                resoultImage.at(index) = resoult.r*255;
                resoultImage.at(index+1) = resoult.g*255;
                resoultImage.at(index+2) = resoult.b*255;
                
            }

        }

    }

}

glm::vec3 PBRNeutralToneMapping( glm::vec3 color ) {
  const float startCompression = 0.8 - 0.04;
  const float desaturation = 0.15;

  float x = std::min(color.r, std::min(color.g, color.b));
  float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
  color -= offset;

  float peak = std::max(color.r, std::max(color.g, color.b));
  if (peak < startCompression) return color;

  const float d = 1. - startCompression;
  float newPeak = 1. - d * d / (peak + d - startCompression);
  color *= newPeak / peak;

  float g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
  return mix(color, newPeak * glm::vec3(1, 1, 1), g);
}