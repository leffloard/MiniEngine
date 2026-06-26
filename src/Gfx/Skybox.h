#pragma once

namespace MiniEngine {

// Skybox: procedural gradient cubemap + cube mesh. Draw last with depth trick.
class Skybox {
public:
    Skybox() = default;
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&& other) noexcept;
    Skybox& operator=(Skybox&& other) noexcept;

    // Create with procedural gradient (top = sky color, bottom = horizon). No file load.
    void createProcedural(const float* topColorRGB, const float* bottomColorRGB);
    void draw() const;
    bool isValid() const { return m_vao != 0 && m_cubemap != 0; }

private:
    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_cubemap = 0;
    static constexpr int SKYBOX_VERTEX_COUNT = 36;
};

} // namespace MiniEngine
