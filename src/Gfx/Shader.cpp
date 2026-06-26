#include "Gfx/Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace MiniEngine {

Shader::~Shader() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

Shader::Shader(Shader&& other) noexcept : m_program(other.m_program), m_uniformCache(std::move(other.m_uniformCache)) {
    other.m_program = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_program) glDeleteProgram(m_program);
        m_program = other.m_program;
        m_uniformCache = std::move(other.m_uniformCache);
        other.m_program = 0;
    }
    return *this;
}

bool Shader::readFile(const std::string& path, std::string& out) {
    std::ifstream f(path);
    if (!f.is_open()) return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    return true;
}

unsigned int Shader::compileShader(unsigned int type, const char* src, std::string& log) {
    unsigned int sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        log.resize(static_cast<size_t>(len), '\0');
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

unsigned int Shader::linkProgram(unsigned int vs, unsigned int fs, std::string& log) {
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        log.resize(static_cast<size_t>(len), '\0');
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

bool Shader::loadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vsSrc, fsSrc;
    if (!readFile(vertexPath, vsSrc)) {
        std::cerr << "Shader: could not open vertex shader: " << vertexPath << "\n";
        return false;
    }
    if (!readFile(fragmentPath, fsSrc)) {
        std::cerr << "Shader: could not open fragment shader: " << fragmentPath << "\n";
        return false;
    }
    std::string log;
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vsSrc.c_str(), log);
    if (!vs) {
        std::cerr << "Vertex shader compile failed:\n" << log << "\n";
        return false;
    }
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fsSrc.c_str(), log);
    if (!fs) {
        std::cerr << "Fragment shader compile failed:\n" << log << "\n";
        glDeleteShader(vs);
        return false;
    }
    unsigned int prog = linkProgram(vs, fs, log);
    if (!prog) {
        std::cerr << "Program link failed:\n" << log << "\n";
        return false;
    }
    if (m_program) glDeleteProgram(m_program);
    m_program = prog;
    m_uniformCache.clear();
    return true;
}

void Shader::use() const {
    glUseProgram(m_program);
}

int Shader::getUniformLocation(const std::string& name) {
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end()) return it->second;
    int loc = glGetUniformLocation(m_program, name.c_str());
    m_uniformCache[name] = loc;
    return loc;
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::setFloat(const std::string& name, float value) {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform1f(loc, value);
}

void Shader::setInt(const std::string& name, int value) {
    int loc = getUniformLocation(name);
    if (loc >= 0) glUniform1i(loc, value);
}

} // namespace MiniEngine
