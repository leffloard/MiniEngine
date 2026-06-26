#pragma once

#include <glm/glm.hpp>

namespace MiniEngine {

struct SceneLighting;
class Shader;
class Camera;
struct Entity;
class Scene;
class Skybox;

class Renderer {
public:
    void setShader(Shader* shader) { m_shader = shader; }
    void setInstancedShader(Shader* shader) { m_instancedShader = shader; }
    void setSkyboxShader(Shader* shader) { m_skyboxShader = shader; }
    void setGridEnabled(bool enabled) { m_gridEnabled = enabled; }
    bool gridEnabled() const { return m_gridEnabled; }
    void setWireframe(bool enabled) { m_wireframe = enabled; }
    bool wireframe() const { return m_wireframe; }

    void beginFrame(const glm::vec3& clearColor);
    void drawEntity(const Entity& entity, const Camera& camera, const SceneLighting& lighting, float aspectRatio = 16.0f / 9.0f);
    void drawScene(const Scene& scene, const Camera& camera, const SceneLighting& lighting, float aspectRatio = 16.0f / 9.0f);
    void drawSkybox(const Skybox* skybox, const Camera& camera, float aspectRatio = 16.0f / 9.0f);

private:
    Shader* m_shader        = nullptr;
    Shader* m_instancedShader = nullptr;
    Shader* m_skyboxShader  = nullptr;
    bool    m_gridEnabled   = true;
    bool    m_wireframe     = false;
};

} // namespace MiniEngine
