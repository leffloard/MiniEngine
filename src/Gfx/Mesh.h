#pragma once

namespace MiniEngine {

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void draw(unsigned int vertexCount) const;
    void drawInstanced(unsigned int vertexCount, unsigned int instanceCount) const;
    unsigned int vertexCount() const { return m_vertexCount; }
    unsigned int vao() const { return m_vao; }

    static Mesh createCube();
    static Mesh createGroundPlane(float halfSizeXZ);

private:
    Mesh(unsigned int vao, unsigned int vbo, unsigned int vertexCount);

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_vertexCount = 0;
};

} // namespace MiniEngine
