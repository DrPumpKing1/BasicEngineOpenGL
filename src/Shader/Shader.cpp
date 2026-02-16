#include "Shader.h"

std::string ShaderTypeToString(ShaderType type)
{
    switch (type)
    {
        case ShaderType::VERTEX:
            return "VERTEX";
        case ShaderType::FRAGMENT:
            return "FRAGMENT";
        case ShaderType::GEOMETRY:
            return "GEOMETRY";
        case ShaderType::COMPUTE:
            return "COMPUTE";
        default:
            return "UNKNOWN";
    }
}

Shader::Shader(const std::string &path, ShaderType type) : type(type)
{
    const char *source = FileReader::ReadFile(vertexPath).c_str();

    ID {glCreateShader(GL_VERTEX_SHADER)};
    glShaderSource(ID, 1, &source, nullptr);
    glCompileShader(ID);
    CheckCompileErrors(ID, type);
}

Shader::~Shader()
{
    glDeleteShader(ID);
}

void Shader::CheckCompileErrors() const
{
    int success;
    char infoLog[INFO_LOG_SIZE];
    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(ID, INFO_LOG_SIZE, nullptr, infoLog);
        std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << ShaderTypeToString(type) << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
}