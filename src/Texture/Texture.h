#pragma once

#include <glad/gl.h>
#include <stb/stb_image.h>

#include "../Shader/ShaderProgram.h"
#include <string>

enum class TextureType {
    DIFFUSE,
    SPECULAR,
    NORMAL,
    HEIGHT,
    DISPLACEMENT,
};

class Texture
{
private:
    GLuint ID;
    TextureType type;
    std::string path;
    GLenum unit;
public:
    Texture(const char *path, TextureType type, GLenum unit, bool transparency = false)
    ~Texture();

    void Bind() const;
    void Unbind() const;

    void SetShaderUniform(const ShaderProgram &shader, const std::string &uniformName) const;
};
