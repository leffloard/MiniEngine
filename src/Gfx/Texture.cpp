#include "Gfx/Texture.h"
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <cstdint>

#ifdef STB_IMAGE_AVAILABLE
#  define STB_IMAGE_IMPLEMENTATION
#  include "stb_image.h"
#endif

namespace MiniEngine {

Texture2D::~Texture2D() {
    if (m_id) {
        glDeleteTextures(1, &m_id);
        m_id = 0;
    }
}

Texture2D::Texture2D(Texture2D&& other) noexcept
    : m_id(other.m_id), m_width(other.m_width), m_height(other.m_height) {
    other.m_id = 0;
    other.m_width = other.m_height = 0;
}

Texture2D& Texture2D::operator=(Texture2D&& other) noexcept {
    if (this != &other) {
        if (m_id) glDeleteTextures(1, &m_id);
        m_id = other.m_id;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_id = 0;
        other.m_width = other.m_height = 0;
    }
    return *this;
}

Texture2D::Texture2D(unsigned int id, int width, int height)
    : m_id(id), m_width(width), m_height(height) {}

bool Texture2D::loadFromFile(const std::string& path) {
#ifdef STB_IMAGE_AVAILABLE
    stbi_set_flip_vertically_on_load(1);
    int w = 0, h = 0, ch = 0;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data) {
        std::cerr << "Texture: failed to load " << path << " - " << stbi_failure_reason() << "\n";
        return false;
    }

    if (m_id) glDeleteTextures(1, &m_id);
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
    m_width = w;
    m_height = h;
    return true;
#else
    (void)path;
    std::cerr << "Texture: stb_image.h not available. Place it in vendor/ to enable file loading.\n";
    return false;
#endif
}

Texture2D Texture2D::createCheckerboard(int tilesPerSide, int texSize) {
    std::vector<uint8_t> pixels(static_cast<size_t>(texSize) * texSize * 4);
    const int tilePixels = texSize / tilesPerSide;
    for (int y = 0; y < texSize; ++y) {
        for (int x = 0; x < texSize; ++x) {
            int tx = x / tilePixels;
            int ty = y / tilePixels;
            bool light = ((tx + ty) % 2) == 0;
            uint8_t v = light ? 220u : 80u;
            size_t i = (static_cast<size_t>(y) * texSize + x) * 4;
            pixels[i + 0] = v;
            pixels[i + 1] = v;
            pixels[i + 2] = v;
            pixels[i + 3] = 255;
        }
    }

    unsigned int id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return Texture2D(id, texSize, texSize);
}

void Texture2D::bind(unsigned int textureUnit) const {
    if (m_id == 0) return;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

} // namespace MiniEngine
