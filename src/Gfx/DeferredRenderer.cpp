#include "Gfx/DeferredRenderer.h"
#include "Gfx/Shader.h"
#include "Gfx/Mesh.h"
#include "Gfx/Material.h"
#include "Gfx/Texture.h"
#include "Gfx/Lighting.h"
#include "Scene/Camera.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstring>

namespace MiniEngine {

DeferredRenderer::~DeferredRenderer() {
    destroyGBuffer();
    if (m_quadVAO) glDeleteVertexArrays(1, &m_quadVAO);
    if (m_quadVBO) glDeleteBuffers(1, &m_quadVBO);
    m_quadVAO = m_quadVBO = 0;
}

void DeferredRenderer::destroyGBuffer() {
    if (m_gPosition) glDeleteTextures(1, &m_gPosition);
    if (m_gNormal)   glDeleteTextures(1, &m_gNormal);
    if (m_gAlbedo)   glDeleteTextures(1, &m_gAlbedo);
    if (m_gMaterial) glDeleteTextures(1, &m_gMaterial);
    if (m_depthRbo)  glDeleteRenderbuffers(1, &m_depthRbo);
    if (m_fbo)       glDeleteFramebuffers(1, &m_fbo);
    m_gPosition = m_gNormal = m_gAlbedo = m_gMaterial = m_depthRbo = m_fbo = 0;
}

void DeferredRenderer::createGBuffer(int width, int height) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // gPosition (RGB16F)
    glGenTextures(1, &m_gPosition);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

    // gNormal (RGB16F)
    glGenTextures(1, &m_gNormal);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

    // gAlbedo (RGBA8) - .a = isGround for grid
    glGenTextures(1, &m_gAlbedo);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedo, 0);

    // gMaterial (RGBA8: .r=metal, .g=rough)
    glGenTextures(1, &m_gMaterial);
    glBindTexture(GL_TEXTURE_2D, m_gMaterial);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gMaterial, 0);

    // depth renderbuffer
    glGenRenderbuffers(1, &m_depthRbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRbo);

    GLenum drawBufs[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, drawBufs);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "DeferredRenderer: G-Buffer FBO incomplete\n";
        destroyGBuffer();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DeferredRenderer::createFullscreenQuad() {
    float quad[] = {
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f
    };
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool DeferredRenderer::init(int width, int height, const char* /*shaderDir*/) {
    m_width  = width;
    m_height = height;
    if (m_width <= 0 || m_height <= 0) return false;
    createGBuffer(m_width, m_height);
    if (!m_fbo) return false;
    createFullscreenQuad();
    return true;
}

void DeferredRenderer::resize(int width, int height) {
    if (width <= 0 || height <= 0) return;
    if (width == m_width && height == m_height) return;
    m_width  = width;
    m_height = height;
    destroyGBuffer();
    createGBuffer(m_width, m_height);
}

void DeferredRenderer::geometryPass(const Scene& scene, const Camera& camera, float aspectRatio) {
    if (!m_fbo || !m_gbufferShader || !m_gbufferShader->isValid()) return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_width, m_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.viewMatrix();
    glm::mat4 proj = camera.projectionMatrix(aspectRatio);

    m_gbufferShader->use();
    m_gbufferShader->setMat4("uView", view);
    m_gbufferShader->setMat4("uProj", proj);
    m_gbufferShader->setInt("uGridEnabled", m_gridEnabled ? 1 : 0);

    const auto& entities = scene.entities();
    for (size_t i = 0; i < entities.size(); ++i) {
        const Entity& entity = entities[i];
        if (!entity.visible || !entity.mesh) continue;

        glm::mat4 model = entity.transform.matrix();
        m_gbufferShader->setMat4("uModel", model);

        glm::vec3 baseColor = (entity.colorOverride.x >= 0.0f) ? entity.colorOverride : entity.material.baseColor;
        m_gbufferShader->setVec3("uBaseColor", baseColor);
        m_gbufferShader->setFloat("uMetal", entity.material.metal);
        m_gbufferShader->setFloat("uRough", entity.material.rough);
        m_gbufferShader->setInt("uIsGround", entity.material.isGround);

        bool useTex = (entity.material.diffuseTexture != nullptr && entity.material.diffuseTexture->isValid());
        m_gbufferShader->setInt("uUseTexture", useTex ? 1 : 0);
        if (useTex) {
            entity.material.diffuseTexture->bind(0);
            m_gbufferShader->setInt("uDiffuseTex", 0);
        }

        entity.mesh->draw(entity.mesh->vertexCount());
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::lightingPass(const SceneLighting& lighting, const Camera& camera, float aspectRatio) {
    if (!m_lightingShader || !m_lightingShader->isValid() || !m_quadVAO) return;

    glViewport(0, 0, m_width, m_height);
    glDisable(GL_DEPTH_TEST);
    m_lightingShader->use();
    m_lightingShader->setVec3("uViewPos", camera.position());
    m_lightingShader->setVec3("uDirLightDir", lighting.dirLightDir);
    m_lightingShader->setVec3("uDirLightColor", lighting.dirLightColor);
    m_lightingShader->setVec3("uPointLightPos", lighting.pointLightPos);
    m_lightingShader->setVec3("uPointLightColor", lighting.pointLightColor);
    m_lightingShader->setFloat("uSpecularIntensity", lighting.specularIntensity);
    m_lightingShader->setInt("uGridEnabled", m_gridEnabled ? 1 : 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gPosition);
    m_lightingShader->setInt("uGPosition", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gNormal);
    m_lightingShader->setInt("uGNormal", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gAlbedo);
    m_lightingShader->setInt("uGAlbedo", 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_gMaterial);
    m_lightingShader->setInt("uGMaterial", 3);

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glEnable(GL_DEPTH_TEST);
}

} // namespace MiniEngine
