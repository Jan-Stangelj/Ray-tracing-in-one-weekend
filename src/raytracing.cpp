#include "raytracing.hpp"

#include "glm/geometric.hpp"
#include "material.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include "random.hpp"

#include <algorithm>
#include <optional>

namespace rt {

    void perSample(glm::vec3& beauty, glm::vec3& albedo, glm::vec3& normal, rt::ray r, const rt::scene& scene, uint32_t& seed) {

        glm::vec3 incomingLight(0.0f);
        glm::vec3 rayColur(1.0f);

        bool foundAlbedo = false;

        for (unsigned int i = 0; i < rt::maxBounces; i++) {
            rt::hitInfo hit;
            
            if (scene.hit(r, hit)) {
                // Hit shader

                const rt::material& material = scene.materials.at(hit.material);

                // First surface sets normal
                if (i == 0) normal += hit.normal.value();

                // First non translucent surface sets albedo
                if (!foundAlbedo) {
                    if (auto materialAlbedo = material.albedo()) {
                        albedo += *materialAlbedo;
                        foundAlbedo = true;
                    }
                }

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

        beauty += incomingLight;
    }

    void render(std::vector<float>& beautyImage, std::vector<float>& albedoImage, std::vector<float>& normalImage, const rt::camera& cam, const rt::scene& scene) {
        // Renders all the pixels in parallel
        #pragma omp parallel for
        for (unsigned int y = 0; y < rt::resolutionY; y++) {

            for (unsigned int x = 0; x < rt::resolutionX; x++) {

                rt::ray ray = cam.genRay(x, y);

                glm::vec3 beauty(0.0f);
                glm::vec3 albedo(0.0f);
                glm::vec3 normal(0.0f);

                uint32_t seed = rt::PCGhash(x * 1973u + y * 9277u + 0x68bc21ebu);

                for (unsigned int i = 0; i < rt::samples; i++) {
                    glm::vec2 randAA(randomFloat(seed), randomFloat(seed));
                    glm::vec2 randDOF = randomVec2InUnitDisk(seed);

                    glm::vec3 AAJiggle = (cam.getRight() * randAA.x + cam.getUp() * randAA.y) * rt::AAjiggle;

                    glm::vec3 DOFjiggle = (cam.getRight() * randDOF.x + cam.getUp() * randDOF.y) * rt::DOFjiggle;
                    glm::vec3 DOForigin = ray.origin() + DOFjiggle;
                    glm::vec3 DOFdirection = glm::normalize(ray.at(rt::DOFfocus) - DOForigin);

                    rt::ray r(DOForigin + AAJiggle, DOFdirection);

                    perSample(beauty, albedo, normal, r, scene, seed);
                }

                beauty /= rt::samples;
                albedo /= rt::samples;
                normal /= rt::samples;

                uint32_t index = (y * rt::resolutionX + x)*3;

                beautyImage.at(index) = beauty.r;
                beautyImage.at(index+1) = beauty.g;
                beautyImage.at(index+2) = beauty.b;

                albedoImage.at(index) = albedo.r;
                albedoImage.at(index+1) = albedo.g;
                albedoImage.at(index+2) = albedo.b;

                normalImage.at(index) = normal.r;
                normalImage.at(index+1) = normal.g;
                normalImage.at(index+2) = normal.b;
                
            }

        }

    }

}