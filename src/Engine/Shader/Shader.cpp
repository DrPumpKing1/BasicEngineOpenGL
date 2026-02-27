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

GLenum ShaderTypeToGL(ShaderType type)
{
    switch (type)
    {
        case ShaderType::VERTEX:
            return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        case ShaderType::GEOMETRY:
            return GL_GEOMETRY_SHADER;
        case ShaderType::COMPUTE:
            return GL_COMPUTE_SHADER;
        default:
            throw std::invalid_argument("Unknown shader type");
    }
}

Shader::Shader(const std::string &path, ShaderType type) : type(type)
{
    std::string shaderSource = FileReader::ReadFile(path);
    const char *source = shaderSource.c_str();

    ID = glCreateShader(ShaderTypeToGL(type));
    glShaderSource(ID, 1, &source, nullptr);
    glCompileShader(ID);
    CheckCompileErrors();
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