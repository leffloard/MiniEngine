#pragma once

namespace MiniEngine {

struct SceneLighting;
class Shader;
class Camera;
class Scene;
class Mesh;

// Deferred pipeline: G-Buffer (geometry pass) + fullscreen lighting pass.
// RAII: FBO and attachments owned. Use when F9 deferred is ON.
class DeferredRenderer {
public:
    DeferredRenderer() = default;
    ~DeferredRenderer();

    DeferredRenderer(const DeferredRenderer&) = delete;
    DeferredRenderer& operator=(const DeferredRenderer&) = delete;

    // Call after GL context is ready. Loads shaders from shaderDir. Returns false if shaders missing.
    bool init(int width, int height, const char* shaderDir);
    void resize(int width, int height);

    void setGbufferShader(Shader* shader) { m_gbufferShader = shader; }
    void setLightingShader(Shader* shader) { m_lightingShader = shader; }
    void setGridEnabled(bool enabled) { m_gridEnabled = enabled; }

    // Geometry pass: render scene into G-Buffer (position, normal, albedo, material).
    void geometryPass(const Scene& scene, const Camera& camera, float aspectRatio);
    // Lighting pass: fullscreen quad, sample G-Buffer, output to bound FBO (usually 0).
    void lightingPass(const SceneLighting& lighting, const Camera& camera, float aspectRatio);

    bool isValid() const { return m_fbo != 0; }
    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    void createGBuffer(int width, int height);
    void destroyGBuffer();
    void createFullscreenQuad();

    Shader* m_gbufferShader  = nullptr;
    Shader* m_lightingShader = nullptr;
    bool    m_gridEnabled    = true;

    unsigned int m_fbo       = 0;
    unsigned int m_gPosition = 0;
    unsigned int m_gNormal   = 0;
    unsigned int m_gAlbedo   = 0;
    unsigned int m_gMaterial = 0;
    unsigned int m_depthRbo  = 0;
    unsigned int m_quadVAO   = 0;
    unsigned int m_quadVBO   = 0;

    int m_width  = 0;
    int m_height = 0;
};

} // namespace MiniEngine
