#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

const int UNIFORM_CACHE_INITIAL_SIZE = 4;

class ShaderUniforms
{
private:
    GLuint programID;
public:
    ShaderUniforms(GLuint programID);
    ~ShaderUniforms();

    GLint GetUniformLocation(const std::string &name) const;
private:
    mutable std::unordered_map<std::string, GLint> uniformLocations;
};