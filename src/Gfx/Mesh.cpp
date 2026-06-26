#include "Gfx/Mesh.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cstddef>

namespace MiniEngine {

Mesh::~Mesh() {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_vertexCount(other.m_vertexCount) {
    other.m_vao = other.m_vbo = other.m_vertexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_vertexCount = other.m_vertexCount;
        other.m_vao = other.m_vbo = other.m_vertexCount = 0;
    }
    return *this;
}

Mesh::Mesh(unsigned int vao, unsigned int vbo, unsigned int vertexCount)
    : m_vao(vao), m_vbo(vbo), m_vertexCount(vertexCount) {}

void Mesh::draw(unsigned int vertexCount) const {
    if (m_vao == 0) return;
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount));
    glBindVertexArray(0);
}

void Mesh::drawInstanced(unsigned int vertexCount, unsigned int instanceCount) const {
    if (m_vao == 0) return;
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount), static_cast<GLsizei>(instanceCount));
    glBindVertexArray(0);
}

// Vertex layout: position(3), normal(3), uv(2) = 8 floats
static constexpr int FLOATS_PER_VERTEX = 8;

Mesh Mesh::createCube() {
    // 36 vertices: pos(3) + normal(3) + uv(2) per vertex
    float cube[] = {
        // back face (-Z)
        -0.5f,-0.5f,-0.5f,  0,0,-1,  0,0,  0.5f,-0.5f,-0.5f,  0,0,-1,  1,0,  0.5f, 0.5f,-0.5f,  0,0,-1,  1,1,
         0.5f, 0.5f,-0.5f,  0,0,-1,  1,1, -0.5f, 0.5f,-0.5f,  0,0,-1,  0,1, -0.5f,-0.5f,-0.5f,  0,0,-1,  0,0,
        // front face (+Z)
        -0.5f,-0.5f, 0.5f,  0,0, 1,  0,0,  0.5f,-0.5f, 0.5f,  0,0, 1,  1,0,  0.5f, 0.5f, 0.5f,  0,0, 1,  1,1,
         0.5f, 0.5f, 0.5f,  0,0, 1,  1,1, -0.5f, 0.5f, 0.5f,  0,0, 1,  0,1, -0.5f,-0.5f, 0.5f,  0,0, 1,  0,0,
        // left (-X)
        -0.5f, 0.5f, 0.5f, -1,0,0,  1,0, -0.5f, 0.5f,-0.5f, -1,0,0,  0,0, -0.5f,-0.5f,-0.5f, -1,0,0,  0,1,
        -0.5f,-0.5f,-0.5f, -1,0,0,  0,1, -0.5f,-0.5f, 0.5f, -1,0,0,  1,1, -0.5f, 0.5f, 0.5f, -1,0,0,  1,0,
        // right (+X)
         0.5f, 0.5f, 0.5f,  1,0,0,  0,0,  0.5f, 0.5f,-0.5f,  1,0,0,  1,0,  0.5f,-0.5f,-0.5f,  1,0,0,  1,1,
         0.5f,-0.5f,-0.5f,  1,0,0,  1,1,  0.5f,-0.5f, 0.5f,  1,0,0,  0,1,  0.5f, 0.5f, 0.5f,  1,0,0,  0,0,
        // bottom (-Y)
        -0.5f,-0.5f,-0.5f,  0,-1,0,  0,1,  0.5f,-0.5f,-0.5f,  0,-1,0,  1,1,  0.5f,-0.5f, 0.5f,  0,-1,0,  1,0,
         0.5f,-0.5f, 0.5f,  0,-1,0,  1,0, -0.5f,-0.5f, 0.5f,  0,-1,0,  0,0, -0.5f,-0.5f,-0.5f,  0,-1,0,  0,1,
        // top (+Y)
        -0.5f, 0.5f,-0.5f,  0, 1,0,  0,0,  0.5f, 0.5f,-0.5f,  0, 1,0,  1,0,  0.5f, 0.5f, 0.5f,  0, 1,0,  1,1,
         0.5f, 0.5f, 0.5f,  0, 1,0,  1,1, -0.5f, 0.5f, 0.5f,  0, 1,0,  0,1, -0.5f, 0.5f,-0.5f,  0, 1,0,  0,0
    };
    constexpr unsigned int vertexCount = 36;

    unsigned int vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return Mesh(vao, vbo, vertexCount);
}

Mesh Mesh::createGroundPlane(float halfSizeXZ) {
    // Two triangles, 6 vertices. UVs tile in world (e.g. 1 unit = 1 tile)
    float g[] = {
        -halfSizeXZ, 0.0f, -halfSizeXZ,  0,1,0,  0, 0,
         halfSizeXZ, 0.0f, -halfSizeXZ,  0,1,0,  2.f * halfSizeXZ, 0,
         halfSizeXZ, 0.0f,  halfSizeXZ,  0,1,0,  2.f * halfSizeXZ, 2.f * halfSizeXZ,
         halfSizeXZ, 0.0f,  halfSizeXZ,  0,1,0,  2.f * halfSizeXZ, 2.f * halfSizeXZ,
        -halfSizeXZ, 0.0f,  halfSizeXZ,  0,1,0,  0, 2.f * halfSizeXZ,
        -halfSizeXZ, 0.0f, -halfSizeXZ,  0,1,0,  0, 0
    };
    constexpr unsigned int vertexCount = 6;

    unsigned int vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g), g, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FLOATS_PER_VERTEX * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return Mesh(vao, vbo, vertexCount);
}

} // namespace MiniEngine
