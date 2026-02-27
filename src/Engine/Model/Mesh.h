#pragma once

#include <glad/gl.h>

#include "../Vertex/Vertex.h"
#include "../Shader/ShaderProgram.h"
#include "../Texture/Texture.h"

#include <vector>

class Mesh 
{
private:
    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    GLuint VAO, VBO, EBO;
public:
    Mesh(std::vector<Vertex3D> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    ~Mesh();

    void Draw(ShaderProgram& shaderProgram);
};