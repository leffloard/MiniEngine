#pragma once

#include <string>

namespace MiniEngine {

// RAII OpenGL 2D texture. Repeat wrap + linear mipmaps.
class Texture2D {
public:
    Texture2D() = default;
    ~Texture2D();

    Texture2D(const Texture2D&) = delete;
    Texture2D& operator=(const Texture2D&) = delete;
    Texture2D(Texture2D&& other) noexcept;
    Texture2D& operator=(Texture2D&& other) noexcept;

    // Load from file (PNG/JPG). Returns true on success.
    bool loadFromFile(const std::string& path);
    // Create a tiled checker pattern (e.g. for ground). No file needed.
    static Texture2D createCheckerboard(int tilesPerSide = 8, int texSize = 256);

    void bind(unsigned int textureUnit = 0) const;
    unsigned int id() const { return m_id; }
    bool isValid() const { return m_id != 0; }
    int width() const { return m_width; }
    int height() const { return m_height; }

private:
    Texture2D(unsigned int id, int width, int height);

    unsigned int m_id = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace MiniEngine
