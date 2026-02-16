#include "Texture.h"

Texture::Texture(const char *path, TextureType type, GLenum unit, bool transparency) : type(type), path(path), unit(unit)
{
    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    std::string fullPath = FileSystem::GetPath(path);
    unsigned char *bytes = stbi_load(fullPath.c_str(), &widthImg, &heightImg, &numColCh, 0);

    glGenTextures(1, &ID);

    Bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum wrapMode = transparency ? GL_CLAMP_TO_EDGE : GL_REPEAT;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    GLint internalformat;
    GLenum format;

    switch(type)
    {
        case TextureType::NORMAL:
            internalformat = GL_RGB;
            break;
        case TextureType::HEIGHT:
        case TextureType::DISPLACEMENT:
            internalformat = GL_RED;
            break;
        case TextureType::DIFFUSE:
        case TextureType::SPECULAR:
            if(numColCh == 1)
            {
                internalformat = GL_R8;
            }
            else if(numColCh == 3)
            {
                internalformat = GL_SRGB8;
            }
            else if(numColCh == 4)
            {
                internalformat = GL_SRGB8_ALPHA8;
            }
            else
            {
                stbi_image_free(bytes);
                throw std::invalid_argument("Unsupported number of color channels in texture");
            }
            break;
        default:
            stbi_image_free(bytes);
            throw std::invalid_argument("Automatic Texture type recognition failed");
    }

    if (numColCh == 1)
    {
        format = GL_RED;
    }
    else if (numColCh == 3)
    {
        format = GL_RGB;
    }
    else if (numColCh == 4)
    {
        format = GL_RGBA;
    }
    else
    {
        stbi_image_free(bytes);
        throw std::invalid_argument("Unsupported number of color channels in texture");
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(bytes);
    Unbind();
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
}

void Texture::Bind() const
{
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetShaderUniform(const ShaderProgram &shader, const std::string &uniformName) const
{
    shader.Bind();
    shader.SetInt(uniformName, unit - GL_TEXTURE0);
}