#pragma once

#include "Scene/Transform.h"
#include "Gfx/Material.h"
#include <glm/glm.hpp>

namespace MiniEngine {

class Mesh;

struct Entity {
    Transform transform;
    Mesh*     mesh     = nullptr;
    Material  material;
    glm::vec3 colorOverride = glm::vec3(-1.0f); // if x>=0, use this instead of material.baseColor
    bool      visible  = true;
};

} // namespace MiniEngine
