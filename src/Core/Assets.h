#pragma once

#include <string>

namespace MiniEngine {

class Shader;
class Texture2D;

// Load and cache shaders/textures by key. Prevents duplicate OpenGL resources.
class Assets {
public:
    // Shader key = "vertPath|fragPath" or a logical name. Loads from file once, caches.
    static Shader* getShader(const std::string& key, const std::string& vertexPath, const std::string& fragmentPath);
    // Texture from file. Key = path or logical name.
    static Texture2D* getTexture(const std::string& key, const std::string& path);
    // Procedural texture (e.g. checker). Key = unique name.
    static Texture2D* getCheckerTexture(const std::string& key, int tilesPerSide = 8, int texSize = 256);

    // Optional: set base directory for relative paths (e.g. SHADER_DIR, texture folder)
    static void setAssetRoot(const std::string& root) { s_assetRoot = root; }
    static const std::string& assetRoot() { return s_assetRoot; }

private:
    static std::string s_assetRoot;
};

} // namespace MiniEngine
