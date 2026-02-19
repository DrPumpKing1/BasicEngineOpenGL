#pragma once

#include <glm/glm.hpp>

struct Vertex2D {
    glm::vec2 texCoords;
    glm::vec2 position;
};

struct Vertex3D {
    glm::vec2 texCoords;
    glm::vec3 position;
    glm::vec3 normal;
};

struct Vertex3DTangent {
    glm::vec2 texCoords;
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};