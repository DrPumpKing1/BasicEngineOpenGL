#pragma once

#include <glad/gl.h>
#include <glm/gtc/constants.hpp>

#include <vector>

#include "../Vertex/Vertex.h"

class Sphere 
{
private:
    unsigned int VAO, VBO, EBO;

    float radius, stacks, slices;

    std::vector<Vertex3D> vertices;
    std::vector<unsigned int> indices;
public:
    Sphere(float radius, unsigned int stacks, unsigned int slices);
    ~Sphere();

    void Draw() const;
private:
    void GenerateMesh();
    Vertex3D GetPointSphere(float u, float v);
};