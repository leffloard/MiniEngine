#include "Gfx/Renderer.h"
#include "Gfx/Shader.h"
#include "Gfx/Mesh.h"
#include "Gfx/Material.h"
#include "Gfx/Texture.h"
#include "Gfx/Lighting.h"
#include "Gfx/Skybox.h"
#include "Scene/Camera.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <unordered_map>
#include <algorithm>

static constexpr unsigned int MAX_INSTANCES = 1024;
static GLuint s_instanceVBO = 0;

static void ensureInstanceVBO() {
    if (s_instanceVBO == 0) {
        glGenBuffers(1, &s_instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, s_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

namespace MiniEngine {

void Renderer::beginFrame(const glm::vec3& clearColor) {
    glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::drawEntity(const Entity& entity, const Camera& camera, const SceneLighting& lighting, float aspectRatio) {
    if (!entity.visible || !m_shader || !m_shader->isValid() || !entity.mesh) return;

    m_shader->use();
    glm::mat4 model = entity.transform.matrix();
    glm::mat4 view  = camera.viewMatrix();
    glm::mat4 proj  = camera.projectionMatrix(aspectRatio);

    m_shader->setMat4("uModel", model);
    m_shader->setMat4("uView", view);
    m_shader->setMat4("uProj", proj);
    m_shader->setVec3("uViewPos", camera.position());
    m_shader->setVec3("uDirLightDir", lighting.dirLightDir);
    m_shader->setVec3("uDirLightColor", lighting.dirLightColor);
    m_shader->setVec3("uPointLightPos", lighting.pointLightPos);
    m_shader->setVec3("uPointLightColor", lighting.pointLightColor);
    m_shader->setFloat("uSpecularIntensity", lighting.specularIntensity);
    glm::vec3 baseColor = (entity.colorOverride.x >= 0.0f) ? entity.colorOverride : entity.material.baseColor;
    m_shader->setVec3("uBaseColor", baseColor);
    m_shader->setFloat("uMetal", entity.material.metal);
    m_shader->setFloat("uRough", entity.material.rough);
    m_shader->setInt("uIsGround", entity.material.isGround);
    m_shader->setInt("uGridEnabled", m_gridEnabled ? 1 : 0);

    bool useTex = (entity.material.diffuseTexture != nullptr && entity.material.diffuseTexture->isValid());
    m_shader->setInt("uUseTexture", useTex ? 1 : 0);
    if (useTex) {
        entity.material.diffuseTexture->bind(0);
        m_shader->setInt("uDiffuseTex", 0);
    }

    if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    entity.mesh->draw(entity.mesh->vertexCount());
    if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::drawScene(const Scene& scene, const Camera& camera, const SceneLighting& lighting, float aspectRatio) {
    if (!m_shader || !m_shader->isValid()) return;

    // Group entities by (mesh, material) for instancing. Key = mesh pointer; we batch same mesh.
    std::unordered_map<Mesh*, std::vector<size_t>> groups;
    const auto& entities = scene.entities();
    for (size_t i = 0; i < entities.size(); ++i) {
        if (entities[i].mesh && entities[i].visible) groups[entities[i].mesh].push_back(i);
    }

    glm::mat4 view = camera.viewMatrix();
    glm::mat4 proj = camera.projectionMatrix(aspectRatio);

    for (const auto& [mesh, indices] : groups) {
        if (indices.empty()) continue;
        if (indices.size() > 1 && m_instancedShader && m_instancedShader->isValid() && indices.size() <= MAX_INSTANCES) {
            // Instanced path
            ensureInstanceVBO();
            std::vector<glm::mat4> matrices;
            matrices.reserve(indices.size());
            for (size_t i : indices) matrices.push_back(entities[i].transform.matrix());
            glBindBuffer(GL_ARRAY_BUFFER, s_instanceVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, matrices.size() * sizeof(glm::mat4), matrices.data());

            const Entity& first = entities[indices[0]];
            m_instancedShader->use();
            m_instancedShader->setMat4("uView", view);
            m_instancedShader->setMat4("uProj", proj);
            m_instancedShader->setVec3("uViewPos", camera.position());
            m_instancedShader->setVec3("uDirLightDir", lighting.dirLightDir);
            m_instancedShader->setVec3("uDirLightColor", lighting.dirLightColor);
            m_instancedShader->setVec3("uPointLightPos", lighting.pointLightPos);
            m_instancedShader->setVec3("uPointLightColor", lighting.pointLightColor);
            m_instancedShader->setFloat("uSpecularIntensity", lighting.specularIntensity);
            glm::vec3 baseColor = (first.colorOverride.x >= 0.0f) ? first.colorOverride : first.material.baseColor;
            m_instancedShader->setVec3("uBaseColor", baseColor);
            m_instancedShader->setFloat("uMetal", first.material.metal);
            m_instancedShader->setFloat("uRough", first.material.rough);
            m_instancedShader->setInt("uIsGround", first.material.isGround);
            m_instancedShader->setInt("uGridEnabled", m_gridEnabled ? 1 : 0);
            m_instancedShader->setInt("uUseTexture", 0);

            glBindVertexArray(mesh->vao());
            glBindBuffer(GL_ARRAY_BUFFER, s_instanceVBO);
            const GLsizei vec4size = sizeof(glm::vec4);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(3);
            glVertexAttribDivisor(3, 1);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(1 * vec4size));
            glEnableVertexAttribArray(4);
            glVertexAttribDivisor(4, 1);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * vec4size));
            glEnableVertexAttribArray(5);
            glVertexAttribDivisor(5, 1);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * vec4size));
            glEnableVertexAttribArray(6);
            glVertexAttribDivisor(6, 1);

            if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            mesh->drawInstanced(mesh->vertexCount(), static_cast<unsigned int>(indices.size()));
            if (m_wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glVertexAttribDivisor(3, 0);
            glVertexAttribDivisor(4, 0);
            glVertexAttribDivisor(5, 0);
            glVertexAttribDivisor(6, 0);
            glBindVertexArray(0);
        } else {
            for (size_t i : indices) drawEntity(entities[i], camera, lighting, aspectRatio);
        }
    }
}

void Renderer::drawSkybox(const Skybox* skybox, const Camera& camera, float aspectRatio) {
    if (!skybox || !skybox->isValid() || !m_skyboxShader || !m_skyboxShader->isValid()) return;
    glDepthFunc(GL_LEQUAL);
    m_skyboxShader->use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.viewMatrix()));
    glm::mat4 proj = camera.projectionMatrix(aspectRatio);
    m_skyboxShader->setMat4("uView", view);
    m_skyboxShader->setMat4("uProj", proj);
    m_skyboxShader->setInt("uSkybox", 0);
    skybox->draw();
    glDepthFunc(GL_LESS);
}

} // namespace MiniEngine
