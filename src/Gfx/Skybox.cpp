#include "Gfx/Skybox.h"
#include <glad/glad.h>
#include <cstring>
#include <cmath>

namespace MiniEngine {

Skybox::~Skybox() {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_cubemap) glDeleteTextures(1, &m_cubemap);
    m_vao = m_vbo = m_cubemap = 0;
}

Skybox::Skybox(Skybox&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_cubemap(other.m_cubemap) {
    other.m_vao = other.m_vbo = other.m_cubemap = 0;
}

Skybox& Skybox::operator=(Skybox&& other) noexcept {
    if (this != &other) {
        if (m_vao) glDeleteVertexArrays(1, &m_vao);
        if (m_vbo) glDeleteBuffers(1, &m_vbo);
        if (m_cubemap) glDeleteTextures(1, &m_cubemap);
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_cubemap = other.m_cubemap;
        other.m_vao = other.m_vbo = other.m_cubemap = 0;
    }
    return *this;
}

// Cube vertices (position only, 3 floats) for skybox - same order as cube faces
static const float SKYBOX_CUBE[] = {
    -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
};

void Skybox::createProcedural(const float* topColorRGB, const float* bottomColorRGB) {
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_cubemap) glDeleteTextures(1, &m_cubemap);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SKYBOX_CUBE), SKYBOX_CUBE, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const int FACE_SIZE = 64;
    unsigned char* faceData = new unsigned char[FACE_SIZE * FACE_SIZE * 3];
    glGenTextures(1, &m_cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

    for (int face = 0; face < 6; ++face) {
        for (int y = 0; y < FACE_SIZE; ++y) {
            float t = static_cast<float>(y) / static_cast<float>(FACE_SIZE - 1);
            for (int x = 0; x < FACE_SIZE; ++x) {
                size_t i = (static_cast<size_t>(y) * FACE_SIZE + x) * 3;
                faceData[i + 0] = static_cast<unsigned char>(255.0f * (topColorRGB[0] * (1.0f - t) + bottomColorRGB[0] * t));
                faceData[i + 1] = static_cast<unsigned char>(255.0f * (topColorRGB[1] * (1.0f - t) + bottomColorRGB[1] * t));
                faceData[i + 2] = static_cast<unsigned char>(255.0f * (topColorRGB[2] * (1.0f - t) + bottomColorRGB[2] * t));
            }
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_RGB, FACE_SIZE, FACE_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, faceData);
    }
    delete[] faceData;

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::draw() const {
    if (m_vao == 0 || m_cubemap == 0) return;
    glBindVertexArray(m_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
    glDrawArrays(GL_TRIANGLES, 0, SKYBOX_VERTEX_COUNT);
    glBindVertexArray(0);
}

} // namespace MiniEngine
