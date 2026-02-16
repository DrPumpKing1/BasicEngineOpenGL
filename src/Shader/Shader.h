#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "../System/FileSystem.h"
#include "../System/FileReader.h"

const unsigned int INFO_LOG_SIZE = 1024;

enum class ShaderType {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    COMPUTE,
};

struct ShaderLinkInfo {
    GLuint ID;
    ShaderType type;
};

std::string ShaderTypeToString(ShaderType type);
GLenum ShaderTypeToGL(ShaderType type);

class Shader
{
private:
    GLuint ID;
    ShaderType type;
public:
    Shader(const std::string &path, ShaderType type);
    ~Shader();

    ShaderLinkInfo GetLinkInfo() const {
        return {ID, type};
    }
private:
    void CheckCompileErrors() const;
};