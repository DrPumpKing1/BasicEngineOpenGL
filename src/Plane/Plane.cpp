#include "Plane.h"

Plane::~Plane() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Plane::Bind() {
    glBindVertexArray(VAO);
}

void Plane::Unbind() {
    glBindVertexArray(0);
}