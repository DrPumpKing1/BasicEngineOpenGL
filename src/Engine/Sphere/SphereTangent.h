#pragma once

#include <glad/gl.h>
#include <glm/gtc/constants.hpp>

#include <vector>

#include "../Vertex/Vertex.h"

class SphereTangent
{
private:
    unsigned int VAO, VBO;

    float radius, stacks, slices;

    std::vector<Vertex3DTangent> vertices;
public:
    SphereTangent(float radius, unsigned int stacks, unsigned int slices);
    ~SphereTangent();

    void Draw() const;
private:
    void GenerateMesh();
    Vertex3DTangent GetPointSphere(float u, float v);
    void CalculateTangentSpace(Vertex3DTangent& vertexA, Vertex3DTangent& vertexB, Vertex3DTangent& vertexC);
};