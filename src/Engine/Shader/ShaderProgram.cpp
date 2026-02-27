#include "ShaderProgram.h"

ShaderProgram::ShaderProgram() : ID(glCreateProgram()), uniforms(ShaderUniforms(ID)) {}

void ShaderProgram::Compile() {
    glLinkProgram(ID);
    CheckLinkErrors();

    if(vertexShader) {
        vertexShader.reset();
    }
    if(fragmentShader) {
        fragmentShader.reset();
    }
    if(geometryShader) {
        geometryShader.reset();
    }
    if(computeShader) {
        computeShader.reset();
    }
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(ID);
}

void ShaderProgram::AttachShader(std::shared_ptr<Shader> reference)
{
    if(!reference) {
        return;
    }

    ShaderLinkInfo linkInfo {reference->GetLinkInfo()};
    std::shared_ptr<Shader> *shaderPtr;
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
    if(*shaderPtr) {
        std::cerr << "ERROR::SHADER_PROGRAM already has a shader of type: " << ShaderTypeToString(linkInfo.type) << std::endl;
        return;
    }
    *shaderPtr = reference;
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