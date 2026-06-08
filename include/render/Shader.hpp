#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <string_view>

class Shader {
private:
    unsigned int m_programID{ 0 };

    unsigned int CompileSource(unsigned int type, std::string_view source);
    void CheckCompileErrors(unsigned int shaderOrProgram, std::string_view type);

public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    void Bind() const;
    void Unbind() const;

    unsigned int GetId() const noexcept {return m_programID; }

    void SetMat4(const std::string& name, const glm::mat4& matrix) const;
    void SetVec3(const std::string& name, const glm::vec3& vector) const;
    void SetInt(const std::string& name, int value) const;
    void SetBool(const std::string& name, bool value) const;
};

