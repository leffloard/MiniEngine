#include "Core/Assets.h"
#include "Gfx/Shader.h"
#include "Gfx/Texture.h"
#include <unordered_map>

namespace MiniEngine {

std::string Assets::s_assetRoot;

static std::unordered_map<std::string, Shader> s_shaders;
static std::unordered_map<std::string, Texture2D> s_textures;

Shader* Assets::getShader(const std::string& key, const std::string& vertexPath, const std::string& fragmentPath) {
    auto it = s_shaders.find(key);
    if (it != s_shaders.end()) return &it->second;
    Shader s;
    std::string vp = s_assetRoot.empty() ? vertexPath : s_assetRoot + "/" + vertexPath;
    std::string fp = s_assetRoot.empty() ? fragmentPath : s_assetRoot + "/" + fragmentPath;
    if (!s.loadFromFile(vp, fp)) {
        if (!s.loadFromFile(vertexPath, fragmentPath)) return nullptr;
    }
    auto [inserted, ok] = s_shaders.emplace(key, std::move(s));
    return &inserted->second;
}

Texture2D* Assets::getTexture(const std::string& key, const std::string& path) {
    auto it = s_textures.find(key);
    if (it != s_textures.end()) return &it->second;
    Texture2D tex;
    std::string fullPath = s_assetRoot.empty() ? path : s_assetRoot + "/" + path;
    if (!tex.loadFromFile(fullPath)) {
        if (!tex.loadFromFile(path)) return nullptr;
    }
    auto [inserted, ok] = s_textures.emplace(key, std::move(tex));
    return &inserted->second;
}

Texture2D* Assets::getCheckerTexture(const std::string& key, int tilesPerSide, int texSize) {
    auto it = s_textures.find(key);
    if (it != s_textures.end()) return &it->second;
    Texture2D tex = Texture2D::createCheckerboard(tilesPerSide, texSize);
    auto [inserted, ok] = s_textures.emplace(key, std::move(tex));
    return &inserted->second;
}

} // namespace MiniEngine
