#pragma once

#include <glm/glm.hpp>

namespace MiniEngine {

// Directional + point light for lit shader. Specular intensity applied in shader.
struct SceneLighting {
    glm::vec3 dirLightDir   = glm::vec3(-0.3f, -0.5f, -0.2f);  // normalized
    glm::vec3 dirLightColor = glm::vec3(0.4f, 0.4f, 0.45f);
    glm::vec3 pointLightPos  = glm::vec3(0.0f, 4.0f, 6.0f);
    glm::vec3 pointLightColor = glm::vec3(0.9f, 0.85f, 0.8f);
    float     specularIntensity = 0.5f;
};

} // namespace MiniEngine
