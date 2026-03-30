#include "Cubemap.h"

Cubemap::Cubemap(std::vector<std::filesystem::path> texturesPaths, GLenum unit) : unit(unit) 
{
    glGenTextures(1, &ID);
    
    Bind();

    int widthImg, heightImg, numColCh;
    for(unsigned int i = 0; i < texturesPaths.size(); i++) 
    {
        unsigned char* bytes = stbi_load(texturesPaths[i].string().c_str(), &widthImg, &heightImg, &numColCh, 0);
        if(bytes) 
        {
            GLenum format;
            GLenum internalFormat;
            if (numColCh == 1)
            {
                internalFormat = GL_R8;
                format = GL_RED;
            }
            else if (numColCh == 3)
            {
                internalFormat = GL_SRGB8;
                format = GL_RGB;
            }
            else if (numColCh == 4)
            {
                internalFormat = GL_SRGB8_ALPHA8;
                format = GL_RGBA;
            }
            else
            {
                stbi_image_free(bytes);
                throw std::invalid_argument("Unsupported number of color channels in texture");
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, widthImg, heightImg, 0, format, GL_UNSIGNED_BYTE, bytes);
        } 
        else 
        {
            std::cerr << "Cubemap Texture failed to load at path: " << texturesPaths[i].string() << std::endl;
        }
        stbi_image_free(bytes);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    Unbind();
}

Cubemap::~Cubemap() 
{
    glDeleteTextures(1, &ID);
}

void Cubemap::Bind() const 
{
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
}

void Cubemap::Unbind() const 
{
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Cubemap::SetShaderUniform(const ShaderProgram &shaderProgram, const std::string &uniformName) const 
{
    shaderProgram.Bind();
    shaderProgram.SetInt(uniformName, unit - GL_TEXTURE0);
}