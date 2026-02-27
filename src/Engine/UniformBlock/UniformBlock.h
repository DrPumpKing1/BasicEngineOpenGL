#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include <string>

#include "../Shader/ShaderProgram.h"

class UniformBlock
{
private:
    GLuint UBO;

    std::string uniformBlockName;
    unsigned int bindingPoint;
    unsigned int size;
public:
    UniformBlock(std::string uniformBlockName, unsigned int bindingPoint, unsigned int size, const void* data = nullptr);
    ~UniformBlock();

    void Bind() const;
    void Unbind() const;

    void UpdateData(const void* data, unsigned int size = 0, unsigned int offset = 0) const;

    void SetShaderUniformBlock(ShaderProgram& shaderProgram) const;
};