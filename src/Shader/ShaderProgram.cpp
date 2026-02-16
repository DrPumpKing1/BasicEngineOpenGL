#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const std::vector<Shader> &shaders)
{
    ID{glCreateProgram()};
    for (const auto &shader : shaders) {
        AttachShader(shader);
    }

    vertexShader = nullptr;
    fragmentShader = nullptr;
    geometryShader = nullptr;
    computeShader = nullptr;

    glLinkProgram(ID);
    CheckLinkErrors();
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(ID);
}

ShaderProgram::AttachShader(const Shader &shader)
{
    ShaderLinkInfo linkInfo {shader.GetLinkInfo()};
    Shader **shaderPtr = nullptr;
    switch (linkInfo.type) {
        case ShaderType::VERTEX:
            shaderPtr = &vertexShader;
            break;
        case ShaderType::FRAGMENT:
            shaderPtr = &fragmentShader;
            break;
        case ShaderType::GEOMETRY:
            shaderPtr = &geometryShader;
            break;
        case ShaderType::COMPUTE:
            shaderPtr = &computeShader;
            break;
    }
    if(!shaderPtr) {
        std::cerr << "ERROR::SHADER_PROGRAM received unknown shader type: " << ShaderTypeToString(linkInfo.type) << std::endl;
        return;
    }
    if(*shaderPtr != nullptr) {
        std::cerr << "ERROR::SHADER_PROGRAM already has a shader of type: " << ShaderTypeToString(linkInfo.type) << std::endl;
        return;
    }
    *shaderPtr = &shader;
    glAttachShader(ID, linkInfo.ID);
}

void ShaderProgram::Bind() const
{
    glUseProgram(ID);
}

void ShaderProgram::Unbind() const
{
    glUseProgram(0);
}

void ShaderProgram::CheckLinkErrors() const
{
    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[INFO_LOG_SIZE];
        glGetProgramInfoLog(ID, INFO_LOG_SIZE, nullptr, infoLog);
        std::cerr << "ERROR::SHADER_PROGRAM linking failed:\n" << infoLog << std::endl;
    }
}