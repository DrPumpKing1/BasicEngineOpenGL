#pragma once

#include <glad/gl.h>

#include "../Vertex/Vertex.h"
#include "../Shader/ShaderProgram.h"
#include "../Texture/Texture.h"

#include <vector>
#include <memory>

class Mesh 
{
private:
    std::vector<Vertex3DTangent> vertices;
    std::vector<unsigned int> indices;
    std::vector<std::weak_ptr<Texture>> textures;

    GLuint VAO, VBO, EBO;
public:
    Mesh();
    ~Mesh();

    void Construct(std::vector<Vertex3DTangent> vertices, std::vector<unsigned int> indices, std::vector<std::weak_ptr<Texture>> textures);
    void Draw(ShaderProgram& shaderProgram);
};