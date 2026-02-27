#include "UniformBlock.h"

UniformBlock::UniformBlock(std::string uniformBlockName, unsigned int bindingPoint, unsigned int size, const void* data) : uniformBlockName(uniformBlockName), bindingPoint(bindingPoint), size(size)
{
    glGenBuffers(1, &UBO);
    Bind();
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    Unbind();

    glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, UBO, 0, size);
}

UniformBlock::~UniformBlock()
{
    glDeleteBuffers(1, &UBO);
}

void UniformBlock::Bind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
}

void UniformBlock::Unbind() const
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBlock::UpdateData(const void* data, unsigned int size, unsigned int offset) const
{
    if(size == 0) {
        size = this->size;
    }

    Bind();
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    Unbind();
}

void UniformBlock::SetShaderUniformBlock(ShaderProgram& shaderProgram) const
{
    unsigned int blockIndex = glGetUniformBlockIndex(shaderProgram.GetID(), uniformBlockName.c_str());
    glUniformBlockBinding(shaderProgram.GetID(), blockIndex, bindingPoint);
}