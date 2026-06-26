#pragma once

#include "Scene/Entity.h"
#include <vector>

namespace MiniEngine {

class Scene {
public:
    std::vector<Entity>& entities() { return m_entities; }
    const std::vector<Entity>& entities() const { return m_entities; }

    void addEntity(Entity e);

    static Scene createDemoScene(class Mesh* cubeMesh, class Mesh* groundMesh,
                                class Texture2D* groundTexture = nullptr,
                                class Texture2D* cubeTexture = nullptr);
    // Add a grid of cubes (e.g. for instancing demo). count = total, arranged in roughly square grid.
    void addCubeGrid(class Mesh* cubeMesh, int count, float spacing, class Texture2D* cubeTexture = nullptr);

private:
    std::vector<Entity> m_entities;
};

} // namespace MiniEngine
