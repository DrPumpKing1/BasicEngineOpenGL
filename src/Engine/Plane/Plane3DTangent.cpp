#include "Plane3DTangent.h"

Plane3DTangent::Plane3DTangent() {
    std::copy(std::begin(PLANE_VERTICES_3D_TANGENT), std::end(PLANE_VERTICES_3D_TANGENT), std::begin(vertices));
    CalculateTangent();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Plane3DTangent::Draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(Vertex3DTangent));
    glBindVertexArray(0);
}

void Plane3DTangent::CalculateTangent() {
    glm::vec3 edge1 = vertices[1].position - vertices[0].position;
    glm::vec3 edge2 = vertices[2].position - vertices[0].position;
    glm::vec2 deltaUV1 = vertices[1].texCoords - vertices[0].texCoords;
    glm::vec2 deltaUV2 = vertices[2].texCoords - vertices[0].texCoords;
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vertices[0].tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    vertices[0].bitangent = f * (deltaUV1.x * edge2 - deltaUV2.x * edge1);
    vertices[1].tangent = vertices[0].tangent;
    vertices[1].bitangent = vertices[0].bitangent;
    vertices[2].tangent = vertices[0].tangent;
    vertices[2].bitangent = vertices[0].bitangent;

    edge1 = vertices[4].position - vertices[3].position;
    edge2 = vertices[5].position - vertices[3].position;
    deltaUV1 = vertices[4].texCoords - vertices[3].texCoords;
    deltaUV2 = vertices[5].texCoords - vertices[3].texCoords;
    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vertices[3].tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    vertices[3].bitangent = f * (deltaUV1.x * edge2 - deltaUV2.x * edge1);
    vertices[4].tangent = vertices[3].tangent;
    vertices[4].bitangent = vertices[3].bitangent;
    vertices[5].tangent = vertices[3].tangent;
    vertices[5].bitangent = vertices[3].bitangent;
}