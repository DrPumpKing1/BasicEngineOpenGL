#include "Sphere.h"

Sphere::Sphere(float radius, unsigned int stacks, unsigned int slices) : radius(radius), stacks(stacks), slices(slices)
{
    GenerateMesh();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, texCoords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Sphere::Draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Sphere::GenerateMesh()
{
    for(unsigned int i = 0; i <= stacks; i++) {
        float u = static_cast<float>(i) / stacks;
        for(unsigned int j = 0; j <= slices; j++) {
            float v = static_cast<float>(j) / slices;
            Vertex3D vertex = GetPointSphere(u, v);
            vertices.push_back(vertex);
        }
    }

    for(unsigned int i = 0; i < stacks; i++) {
        for(unsigned int j = 0; j < slices; j++) {
            unsigned int row1 = i * (slices + 1);
            unsigned int row2 = (i + 1) * (slices + 1);

            indices.push_back(row1 + j);
            indices.push_back(row2 + j);
            indices.push_back(row1 + j + 1);

            indices.push_back(row1 + j + 1);
            indices.push_back(row2 + j);
            indices.push_back(row2 + j + 1);
        }
    }
}

Vertex3D Sphere::GetPointSphere(float u, float v)
{
    Vertex3D vertex;

    float longitude = v * glm::pi<float>();
    float latitude = u * glm::two_pi<float>();

    float x = radius * sin(longitude) * cos(latitude);
    float y = radius * cos(longitude);
    float z = radius * sin(longitude) * sin(latitude);

    vertex.position = glm::vec3(x, y, z);
    vertex.normal = glm::normalize(glm::vec3(x, y, z));
    vertex.texCoords = glm::vec2(u, 1.0f - v);

    return vertex;
}