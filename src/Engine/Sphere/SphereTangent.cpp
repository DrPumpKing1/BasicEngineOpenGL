#include "SphereTangent.h"

SphereTangent::SphereTangent(float radius, unsigned int stacks, unsigned int slices) : radius(radius), stacks(stacks), slices(slices)
{
    GenerateMesh();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3DTangent), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void *)offsetof(Vertex3DTangent, texCoords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void *)offsetof(Vertex3DTangent, position));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void *)offsetof(Vertex3DTangent, normal));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void *)offsetof(Vertex3DTangent, tangent));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3DTangent), (void *)offsetof(Vertex3DTangent, bitangent));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SphereTangent::~SphereTangent()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void SphereTangent::Draw() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}

void SphereTangent::GenerateMesh()
{
    for(unsigned int i = 0; i <= stacks; i++) {
        float v = static_cast<float>(i) / stacks;
        float deltaV = 1.0f / stacks;

        for(unsigned int j = 0; j <= slices; j++) {
            float u = static_cast<float>(j) / slices;
            float deltaU = 1.0f / slices;

            struct Vertex3DTangent quadVertices[6] = {
                GetPointSphere(u, v),
                GetPointSphere(u + deltaU, v),
                GetPointSphere(u, v + deltaV),
                GetPointSphere(u + deltaU, v),
                GetPointSphere(u + deltaU, v + deltaV),
                GetPointSphere(u, v + deltaV),
            };

            CalculateTangentSpace(quadVertices[0], quadVertices[1], quadVertices[2]);
            CalculateTangentSpace(quadVertices[3], quadVertices[4], quadVertices[5]);

            vertices.push_back(quadVertices[0]);
            vertices.push_back(quadVertices[1]);
            vertices.push_back(quadVertices[2]);
            vertices.push_back(quadVertices[3]);
            vertices.push_back(quadVertices[4]);
            vertices.push_back(quadVertices[5]);
        }
    }
}

Vertex3DTangent SphereTangent::GetPointSphere(float u, float v)
{
    Vertex3DTangent vertex;

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

void SphereTangent::CalculateTangentSpace(Vertex3DTangent& vertexA, Vertex3DTangent& vertexB, Vertex3DTangent& vertexC)
{
    glm::vec3 edge1 = vertexB.position - vertexA.position;
    glm::vec3 edge2 = vertexC.position - vertexA.position;

    glm::vec2 deltaUV1 = vertexB.texCoords - vertexA.texCoords;
    glm::vec2 deltaUV2 = vertexC.texCoords - vertexA.texCoords;

    float discriminant = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;

    if(discriminant == 0.0f) {
        vertexA.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
        vertexA.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
        vertexB.tangent = vertexA.tangent;
        vertexB.bitangent = vertexA.bitangent;
        vertexC.tangent = vertexA.tangent;
        vertexC.bitangent = vertexA.bitangent;
        return;
    }

    float f = 1.0f / discriminant;

    vertexA.tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    vertexA.tangent = glm::normalize(vertexA.tangent);
    vertexB.tangent = vertexA.tangent;
    vertexC.tangent = vertexA.tangent;

    vertexA.bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);
    vertexA.bitangent = glm::normalize(vertexA.bitangent);
    vertexB.bitangent = vertexA.bitangent;
    vertexC.bitangent = vertexA.bitangent;
}