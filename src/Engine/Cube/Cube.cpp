#include "Cube.h"

Cube::Cube() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    Bind();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    Unbind();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Cube::~Cube() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Cube::Bind() const {
    glBindVertexArray(VAO);
}

void Cube::Unbind() const {
    glBindVertexArray(0);
}

void Cube::Draw() const {
    Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    Unbind();
}