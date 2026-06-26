#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Gfx/Mesh.h"
#include "Gfx/Texture.h"
#include <glm/glm.hpp>
#include <cmath>

namespace MiniEngine {

void Scene::addEntity(Entity e) {
    m_entities.push_back(std::move(e));
}

Scene Scene::createDemoScene(Mesh* cubeMesh, Mesh* groundMesh,
                            Texture2D* groundTexture, Texture2D* cubeTexture) {
    Scene s;
    if (groundMesh) {
        Entity ground;
        ground.mesh = groundMesh;
        ground.transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
        ground.transform.scale = glm::vec3(1.0f);
        ground.material.baseColor = glm::vec3(0.25f, 0.35f, 0.28f);
        ground.material.isGround = 1;
        ground.material.diffuseTexture = groundTexture;
        s.addEntity(std::move(ground));
    }
    if (cubeMesh) {
        glm::vec3 positions[] = {
            { 0.0f, 0.5f,  0.0f },
            { 2.0f, 0.5f, -1.0f },
            { -2.0f, 0.5f, -2.5f },
            { 1.0f, 0.5f,  2.0f },
            { -3.0f, 0.5f,  1.5f },
            { 0.0f, 1.5f, -4.0f }
        };
        for (const auto& p : positions) {
            Entity cube;
            cube.mesh = cubeMesh;
            cube.transform.position = p;
            cube.transform.scale = glm::vec3(1.0f);
            cube.material.baseColor = glm::vec3(0.95f, 0.90f, 0.30f);
            cube.material.isGround = 0;
            cube.material.diffuseTexture = cubeTexture;
            s.addEntity(std::move(cube));
        }
    }
    return s;
}

void Scene::addCubeGrid(Mesh* cubeMesh, int count, float spacing, Texture2D* cubeTexture) {
    if (!cubeMesh || count <= 0) return;
    int side = static_cast<int>(std::sqrt(static_cast<double>(count)));
    if (side < 1) side = 1;
    int added = 0;
    for (int z = 0; z < side && added < count; ++z) {
        for (int x = 0; x < side && added < count; ++x) {
            Entity cube;
            cube.mesh = cubeMesh;
            float fx = (x - side * 0.5f) * spacing;
            float fz = (z - side * 0.5f) * spacing;
            cube.transform.position = glm::vec3(fx, 0.5f, fz);
            cube.transform.scale = glm::vec3(1.0f);
            cube.material.baseColor = glm::vec3(0.95f, 0.90f, 0.30f);
            cube.material.diffuseTexture = cubeTexture;
            addEntity(std::move(cube));
            ++added;
        }
    }
    for (; added < count; ++added) {
        Entity cube;
        cube.mesh = cubeMesh;
        cube.transform.position = glm::vec3(added * 0.5f, 0.5f, -20.0f);
        cube.transform.scale = glm::vec3(1.0f);
        cube.material.baseColor = glm::vec3(0.95f, 0.90f, 0.30f);
        cube.material.diffuseTexture = cubeTexture;
        addEntity(std::move(cube));
    }
}

} // namespace MiniEngine
