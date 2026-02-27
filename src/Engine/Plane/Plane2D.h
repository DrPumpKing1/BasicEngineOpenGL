#pragma once

#include "Plane.h"

const Vertex2D PLANE_VERTICES_2D[] = {
    {{0.0f, 1.0f}, {-1.0f,  1.0f}}, 
    {{0.0f, 0.0f}, {-1.0f, -1.0f}},
    {{1.0f, 1.0f}, { 1.0f,  1.0f}},
    {{1.0f, 0.0f}, { 1.0f, -1.0f}},
};

class Plane2D : public Plane
{
public:
    Plane2D();

    void Draw() override;
};