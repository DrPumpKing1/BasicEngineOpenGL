#include "Mesh.h"

Mesh::Mesh() {}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    textures.clear();
}

void Mesh::Construct(std::vector<Vertex3DTangent> vertices, std::vector<unsigned int> indices, std::vector<std::weak_ptr<Texture>> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3DTangent), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void*)offsetof(Vertex3DTangent, texCoords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void*)offsetof(Vertex3DTangent, position));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void*)offsetof(Vertex3DTangent, normal));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void*)offsetof(Vertex3DTangent, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void*)offsetof(Vertex3DTangent, bitangent));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw(ShaderProgram& shaderProgram)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    shaderProgram.Bind();
    for (size_t i = 0; i < textures.size(); ++i) 
    {
        if(auto texture = textures[i].lock()) 
        {
            texture->Bind();
            TextureType type = texture->GetType();
            unsigned int textureNr;
            switch(type) {
                case TextureType::DIFFUSE:
                    textureNr = diffuseNr++;
                    break;
                case TextureType::SPECULAR:
                    textureNr = specularNr++;
                    break;
                case TextureType::NORMAL:
                    textureNr = normalNr++;
                    break;
                case TextureType::HEIGHT:
                    textureNr = heightNr++;
                    break;
                default:
                    throw std::invalid_argument("Invalid TextureType");
            }
            texture->SetShaderUniform(shaderProgram, "material." + texture->GetTypeName() + std::to_string(textureNr));
        }
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for (size_t i = 0; i < textures.size(); ++i) 
    {
        if(auto texture = textures[i].lock()) 
        {
            texture->Unbind();
        }
    }
    shaderProgram.Unbind();
}