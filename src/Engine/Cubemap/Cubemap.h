#pragma once

#include <glad/gl.h>
#include <stb/stb_image.h>

#include "../Shader/ShaderProgram.h"

#include <vector>
#include <filesystem>

class Cubemap 
{
private:
    GLuint ID;
    GLenum unit;
public:
    Cubemap(std::vector<std::filesystem::path> texturesPaths, GLenum unit);
    ~Cubemap();

    void Bind() const;
    void Unbind() const;

    void SetShaderUniform(const ShaderProgram &shader, const std::string &uniformName) const;
};