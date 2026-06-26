#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace MiniEngine {

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool loadFromFile(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;

    void setMat4(const std::string& name, const glm::mat4& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setFloat(const std::string& name, float value);
    void setInt(const std::string& name, int value);

    unsigned int id() const { return m_program; }
    bool isValid() const { return m_program != 0; }

private:
    int getUniformLocation(const std::string& name);
    static bool readFile(const std::string& path, std::string& out);
    static unsigned int compileShader(unsigned int type, const char* src, std::string& log);
    static unsigned int linkProgram(unsigned int vs, unsigned int fs, std::string& log);

    unsigned int m_program = 0;
    std::unordered_map<std::string, int> m_uniformCache;
};

} // namespace MiniEngine
