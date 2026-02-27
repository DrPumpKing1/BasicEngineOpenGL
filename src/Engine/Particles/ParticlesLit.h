#pragma once

#include <glad/gl.h>
#include <vector>

#include "../InstancesBuffer/InstancesBuffer.h"
#include "../Vertex/Vertex.h"

const VertexParticlesLit PLANE_VERTICES_3D[] = {
    {{0.0f, 1.0f}, {-1.0f,  1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.0f, 0.0f}, {-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f}, { 1.0f,  1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 0.0f}, { 1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
};

class ParticlesLit
{
private:
    GLuint VAO;
    GLuint VBO;

    InstancesBuffer instancesBuffer;

    const unsigned int instancesCount;
    mutable bool configured = false;
public:
    ParticlesLit(unsigned int instancesCount);
    ~ParticlesLit();

    void Bind() const;
    void Unbind() const;

    BatchView AddProperties(float components, const void* data);
    void ConfigureAttributes() const;

    void Draw(unsigned int count = 0) const;
};