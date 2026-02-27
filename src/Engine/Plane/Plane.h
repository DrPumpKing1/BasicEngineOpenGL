#pragma once

#include <glad/gl.h>

#include "../Vertex/Vertex.h"

class Plane
{
protected:
    GLuint VAO, VBO;
public:
    Plane() {}
    ~Plane();

    virtual void Bind();
    virtual void Unbind();

    virtual void Draw() = 0;
};