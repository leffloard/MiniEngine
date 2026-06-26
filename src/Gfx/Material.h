#pragma once

#include <glm/glm.hpp>

namespace MiniEngine {

class Texture2D;

struct Material {
    glm::vec3   baseColor     = glm::vec3(0.8f);
    float       metal         = 0.0f;
    float       rough         = 0.5f;
    int         isGround      = 0;
    Texture2D*  diffuseTexture = nullptr;  // optional; when set, lit shader samples it
};

} // namespace MiniEngine
