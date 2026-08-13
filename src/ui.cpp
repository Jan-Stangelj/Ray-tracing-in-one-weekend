#include "renderer.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

namespace rt {
    void renderer::displayUI() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Options");

        if (ImGui::BeginTabBar("OptionsTabs")) {

            // ============================================================
            // Rendering
            // ============================================================

            if (ImGui::BeginTabItem("Rendering")) {

                ImGui::InputScalar(
                    "Minimum bounces",
                    ImGuiDataType_U32,
                    &rt::minBounces
                );
                rt::minBounces = std::max(rt::minBounces, 1u);

                ImGui::InputScalar(
                    "Maximum bounces",
                    ImGuiDataType_U32,
                    &rt::maxBounces
                );
                rt::maxBounces = std::max(rt::maxBounces, 1u);

                ImGui::InputScalar(
                    "Samples",
                    ImGuiDataType_U32,
                    &rt::samples
                );
                rt::samples = std::max(rt::samples, 1u);

                ImGui::Separator();

                ImGui::Checkbox("Denoise?", &rt::denoise);

                ImGui::Separator();

                ImGui::SliderFloat(
                    "Vertical FOV",
                    &rt::fovY,
                    30.0f,
                    89.0f,
                    "%.0f"
                );

                ImGui::InputFloat(
                    "Depth of field jiggle",
                    &rt::DOFjiggle
                );
                rt::DOFjiggle = std::max(rt::DOFjiggle, 0.0f);

                ImGui::InputFloat(
                    "Focus",
                    &rt::DOFfocus
                );
                rt::DOFfocus = std::max(rt::DOFfocus, 0.0f);

                ImGui::Separator();

                if (ImGui::Button("Render"))
                    render();

                ImGui::Text(
                    "Rendering took %.2fs",
                    m_renderingTime.count()
                );

                ImGui::Separator();

                ImGui::InputText(
                    "Export path",
                    &rt::exportPath
                );

                ImGui::InputText(
                    "Export filename",
                    &rt::exportFilename
                );

                if (ImGui::Button("Export"))
                    exportRender();

                ImGui::Separator();

                ImGui::Text(
                    "Resolution: %u x %u",
                    rt::resolutionX,
                    rt::resolutionY
                );

                ImGui::EndTabItem();
            }

            // ============================================================
            // Instances
            // ============================================================

            if (ImGui::BeginTabItem("Instances")) {

                static std::string selectedInstance;

                // ============================================================
                // Create new instance
                // ============================================================

                static std::string newInstanceName = "NewInstance";

                ImGui::Text("Create Instance");

                ImGui::InputText(
                    "Name",
                    &newInstanceName
                );

                if (ImGui::Button("Create Instance")) {

                    if (!newInstanceName.empty() &&
                        scene.meshInstances.find(newInstanceName) ==
                            scene.meshInstances.end() &&
                        !scene.meshes.empty() &&
                        !scene.materials.empty()) {

                        const std::string& meshName =
                            scene.meshes.begin()->first;

                        const std::string& materialName =
                            scene.materials.begin()->first;

                        scene.meshInstances.emplace(
                            newInstanceName,
                            rt::meshInstance(
                                meshName,
                                materialName
                            )
                        );

                        selectedInstance = newInstanceName;

                        newInstanceName = "NewInstance";
                    }
                }

                if (scene.meshes.empty() || scene.materials.empty()) {
                    ImGui::TextDisabled(
                        "A mesh and material are required to create an instance."
                    );
                }

                ImGui::Separator();

                // ============================================================
                // Existing instances
                // ============================================================

                if (scene.meshInstances.empty()) {

                    ImGui::Text("No mesh instances in scene.");

                } else {

                    // Select first instance automatically
                    if (selectedInstance.empty() ||
                        scene.meshInstances.find(selectedInstance) ==
                            scene.meshInstances.end()) {

                        selectedInstance =
                            scene.meshInstances.begin()->first;
                    }

                    // --------------------------------------------------------
                    // Instance selector
                    // --------------------------------------------------------

                    std::vector<const char*> instanceNames;
                    instanceNames.reserve(scene.meshInstances.size());

                    for (const auto& [name, instance] : scene.meshInstances)
                        instanceNames.push_back(name.c_str());

                    int selectedIndex = 0;

                    int i = 0;
                    for (const auto& [name, instance] : scene.meshInstances) {

                        if (name == selectedInstance) {
                            selectedIndex = i;
                            break;
                        }

                        ++i;
                    }

                    if (ImGui::Combo(
                            "Instance",
                            &selectedIndex,
                            instanceNames.data(),
                            static_cast<int>(instanceNames.size()))) {

                        auto it = scene.meshInstances.begin();
                        std::advance(it, selectedIndex);

                        selectedInstance = it->first;
                    }

                    auto instanceIt =
                        scene.meshInstances.find(selectedInstance);

                    if (instanceIt != scene.meshInstances.end()) {

                        rt::meshInstance& instance = instanceIt->second;

                        ImGui::Separator();

                        // ----------------------------------------------------
                        // Transform
                        // ----------------------------------------------------

                        ImGui::Text("Transform");

                        ImGui::DragFloat3(
                            "Position",
                            &instance.position.x,
                            0.05f
                        );

                        ImGui::DragFloat3(
                            "Rotation",
                            &instance.rotation.x,
                            1.0f
                        );

                        ImGui::DragFloat3(
                            "Scale",
                            &instance.scale.x,
                            0.05f
                        );

                        ImGui::Separator();

                        // ----------------------------------------------------
                        // Mesh
                        // ----------------------------------------------------

                        if (!scene.meshes.empty()) {

                            std::vector<const char*> meshNames;
                            meshNames.reserve(scene.meshes.size());

                            for (const auto& [name, mesh] : scene.meshes)
                                meshNames.push_back(name.c_str());

                            int meshIndex = 0;

                            int meshI = 0;
                            for (const auto& [name, mesh] : scene.meshes) {

                                if (name == instance.m_mesh) {
                                    meshIndex = meshI;
                                    break;
                                }

                                ++meshI;
                            }

                            if (ImGui::Combo(
                                    "Mesh",
                                    &meshIndex,
                                    meshNames.data(),
                                    static_cast<int>(meshNames.size()))) {

                                auto it = scene.meshes.begin();
                                std::advance(it, meshIndex);

                                instance.m_mesh = it->first;
                            }
                        }

                        // ----------------------------------------------------
                        // Material
                        // ----------------------------------------------------

                        if (!scene.materials.empty()) {

                            std::vector<const char*> materialNames;
                            materialNames.reserve(scene.materials.size());

                            for (const auto& [name, material] : scene.materials)
                                materialNames.push_back(name.c_str());

                            int materialIndex = 0;

                            int materialI = 0;
                            for (const auto& [name, material] : scene.materials) {

                                if (name == instance.m_material) {
                                    materialIndex = materialI;
                                    break;
                                }

                                ++materialI;
                            }

                            if (ImGui::Combo(
                                    "Material",
                                    &materialIndex,
                                    materialNames.data(),
                                    static_cast<int>(materialNames.size()))) {

                                auto it = scene.materials.begin();
                                std::advance(it, materialIndex);

                                instance.m_material = it->first;
                            }
                        }

                        ImGui::Separator();

                        // ----------------------------------------------------
                        // Delete
                        // ----------------------------------------------------

                        if (ImGui::Button("Delete Instance")) {

                            scene.meshInstances.erase(instanceIt);

                            selectedInstance.clear();
                        }
                    }
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // Meshes
            // ============================================================

            if (ImGui::BeginTabItem("Meshes")) {

                static std::string newMeshName = "NewMesh";
                static std::string newMeshPath = "";

                ImGui::Text("Create Mesh");

                ImGui::InputText(
                    "Name",
                    &newMeshName
                );

                ImGui::InputText(
                    "File path",
                    &newMeshPath
                );

                if (ImGui::Button("Create Mesh")) {

                    if (!newMeshName.empty() &&
                        !newMeshPath.empty() &&
                        scene.meshes.find(newMeshName) == scene.meshes.end()) {

                        scene.meshes.emplace(
                            newMeshName,
                            rt::mesh(newMeshPath.c_str())
                        );

                        newMeshName = "NewMesh";
                        newMeshPath.clear();
                    }
                }

                ImGui::Separator();

                ImGui::Text("Meshes");

                for (auto it = scene.meshes.begin();
                    it != scene.meshes.end();) {

                    ImGui::PushID(it->first.c_str());

                    ImGui::Text("%s", it->first.c_str());

                    ImGui::SameLine();

                    if (ImGui::Button("Delete")) {

                        const std::string deletedName = it->first;

                        // Clear references from instances
                        for (auto& [instanceName, instance] :
                            scene.meshInstances) {

                            if (instance.m_mesh == deletedName)
                                instance.m_mesh.clear();
                        }

                        it = scene.meshes.erase(it);

                        ImGui::PopID();
                        continue;
                    }

                    ImGui::PopID();

                    ++it;
                }

                ImGui::EndTabItem();
            }

            // ============================================================
            // Materials
            // ============================================================

            if (ImGui::BeginTabItem("Materials")) {

                static std::string newMaterialName = "NewMaterial";
                static rt::materialType newMaterialType =
                    rt::materialType::LAMBERTIAN;

                // Common parameters
                static glm::vec3 albedo = glm::vec3(1.0f);
                static float roughness = 0.5f;

                // Emissive parameters
                static glm::vec3 emissionColour = glm::vec3(1.0f);
                static float emissionStrength = 1.0f;

                // Dielectric / Clear Coat
                static float IOR = 1.5f;

                // Clear Coat
                static glm::vec3 coatAlbedo = glm::vec3(1.0f);

                // ========================================================
                // Create new material
                // ========================================================

                ImGui::Text("Create Material");

                ImGui::InputText(
                    "Name",
                    &newMaterialName
                );

                // --------------------------------------------------------
                // Material type
                // --------------------------------------------------------

                const char* materialTypes[] = {
                    "Lambertian",
                    "Metal",
                    "Emissive",
                    "Dielectric",
                    "Clear Coat"
                };

                int materialTypeIndex =
                    static_cast<int>(newMaterialType);

                if (ImGui::Combo(
                        "Type",
                        &materialTypeIndex,
                        materialTypes,
                        IM_ARRAYSIZE(materialTypes))) {

                    newMaterialType =
                        static_cast<rt::materialType>(materialTypeIndex);
                }

                // --------------------------------------------------------
                // Parameters
                // --------------------------------------------------------

                switch (newMaterialType) {

                    case rt::materialType::LAMBERTIAN:

                        ImGui::ColorEdit3(
                            "Albedo",
                            &albedo.x
                        );

                        break;


                    case rt::materialType::METAL:

                        ImGui::ColorEdit3(
                            "Albedo",
                            &albedo.x
                        );

                        ImGui::SliderFloat(
                            "Roughness",
                            &roughness,
                            0.0f,
                            1.0f
                        );

                        break;


                    case rt::materialType::EMISSIVE:

                        ImGui::ColorEdit3(
                            "Emission Colour",
                            &emissionColour.x
                        );

                        ImGui::InputFloat(
                            "Emission Strength",
                            &emissionStrength
                        );

                        emissionStrength =
                            std::max(emissionStrength, 0.0f);

                        break;


                    case rt::materialType::DIELECTRIC:

                        ImGui::ColorEdit3(
                            "Albedo",
                            &albedo.x
                        );

                        ImGui::SliderFloat(
                            "Roughness",
                            &roughness,
                            0.0f,
                            1.0f
                        );

                        ImGui::InputFloat(
                            "IOR",
                            &IOR
                        );

                        IOR = std::max(IOR, 1.0f);

                        break;


                    case rt::materialType::CLEARCOAT:

                        ImGui::ColorEdit3(
                            "Albedo",
                            &albedo.x
                        );

                        ImGui::ColorEdit3(
                            "Coat Albedo",
                            &coatAlbedo.x
                        );

                        ImGui::SliderFloat(
                            "Roughness",
                            &roughness,
                            0.0f,
                            1.0f
                        );

                        ImGui::InputFloat(
                            "IOR",
                            &IOR
                        );

                        IOR = std::max(IOR, 1.0f);

                        break;
                }

                // ========================================================
                // Create
                // ========================================================

                if (ImGui::Button("Create Material")) {

                    if (!newMaterialName.empty() &&
                        scene.materials.find(newMaterialName) ==
                            scene.materials.end()) {

                        rt::material newMaterial;

                        switch (newMaterialType) {

                            case rt::materialType::LAMBERTIAN:

                                newMaterial =
                                    rt::createLambertian(albedo);

                                break;


                            case rt::materialType::METAL:

                                newMaterial =
                                    rt::createMetal(
                                        albedo,
                                        roughness
                                    );

                                break;


                            case rt::materialType::EMISSIVE:

                                newMaterial =
                                    rt::createEmissive(
                                        emissionColour,
                                        emissionStrength
                                    );

                                break;


                            case rt::materialType::DIELECTRIC:

                                newMaterial =
                                    rt::createDielectric(
                                        albedo,
                                        roughness,
                                        IOR
                                    );

                                break;


                            case rt::materialType::CLEARCOAT:

                                newMaterial =
                                    rt::createClearCoat(
                                        albedo,
                                        coatAlbedo,
                                        roughness,
                                        IOR
                                    );

                                break;
                        }

                        scene.materials.emplace(
                            newMaterialName,
                            std::move(newMaterial)
                        );

                        newMaterialName = "NewMaterial";
                    }
                }

                ImGui::Separator();

                // ========================================================
                // Existing materials
                // ========================================================

                ImGui::Text("Materials");

                for (auto it = scene.materials.begin();
                    it != scene.materials.end();) {

                    ImGui::PushID(it->first.c_str());

                    ImGui::Text(
                        "%s",
                        it->first.c_str()
                    );

                    ImGui::SameLine();

                    if (ImGui::Button("Delete")) {

                        const std::string deletedName =
                            it->first;

                        // Clear references from instances
                        for (auto& [instanceName, instance] :
                            scene.meshInstances) {

                            if (instance.m_material == deletedName)
                                instance.m_material.clear();
                        }

                        it = scene.materials.erase(it);

                        ImGui::PopID();
                        continue;
                    }

                    ImGui::PopID();

                    ++it;
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}