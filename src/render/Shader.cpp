#include "../../include/render/Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::~Shader() {
    if (m_programID != 0) {
        glDeleteProgram(m_programID);
    }
}

Shader::Shader(Shader&& other) noexcept : m_programID(other.m_programID) {
    other.m_programID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_programID != 0) glDeleteProgram(m_programID);
        m_programID = other.m_programID;
        other.m_programID = 0;
    }
    return *this;
}

bool Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (const std::ifstream::failure& e) {
        std::cerr << "[-] Error: Shader file not successfully read: " << e.what() << '\n';
        return false;
    }

    unsigned int vertexShader = CompileSource(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragmentShader = CompileSource(GL_FRAGMENT_SHADER, fragmentCode);

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertexShader);
    glAttachShader(m_programID, fragmentShader);
    glLinkProgram(m_programID);
    CheckCompileErrors(m_programID, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

unsigned int Shader::CompileSource(unsigned int type, std::string_view source) {
    unsigned int shader = glCreateShader(type);
    const char* srcPtr = source.data();
    glShaderSource(shader, 1, &srcPtr, nullptr);
    glCompileShader(shader);
    CheckCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return shader;
}

void Shader::Bind() const {
    glUseProgram(m_programID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_programID, name.c_str()), 1, &value.x);
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void Shader::CheckCompileErrors(unsigned int shaderOrProgram, std::string_view type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shaderOrProgram, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(shaderOrProgram, 1024, nullptr, infoLog);
            std::cerr << "[-] Error: Shader Compilation of type: " << type << "\n" << infoLog << '\n';
        }
    } else {
        glGetProgramiv(shaderOrProgram, GL_LINK_STATUS, &success);
        if(!success) {
            glGetProgramInfoLog(shaderOrProgram, 1024, nullptr, infoLog);
            std::cerr << "[-] Error: Shader Program Linking of type: " << type << "\n" << infoLog << '\n';
        }
    }
}
    

