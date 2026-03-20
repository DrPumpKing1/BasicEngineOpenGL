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

std::string TextureTypeToString(TextureType type);

class Texture
{
private:
    GLuint ID;
    TextureType type;
    GLenum unit;
public:
    std::string path;

    Texture(const char *path, TextureType type, GLenum unit, bool transparency = false);
    ~Texture();

    void Bind() const;
    void Unbind() const;

    void SetShaderUniform(const ShaderProgram &shader, const std::string &uniformName) const;

    TextureType GetType() const { return type; }
    std::string GetTypeName() const { return TextureTypeToString(type); }
};
