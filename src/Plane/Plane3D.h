#pragma once

#include "Plane.h"

const Vertex3D PLANE_VERTICES_3D[] = {
    {{0.0f, 1.0f}, {-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{0.0f, 0.0f}, {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 1.0f}, { 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    {{1.0f, 0.0f}, { 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
};

class Plane3D : public Plane
{
public:
    Plane3D();

    void Draw() override;
};