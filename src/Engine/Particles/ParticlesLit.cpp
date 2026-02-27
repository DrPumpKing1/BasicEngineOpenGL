#include "ParticlesLit.h"

ParticlesLit::ParticlesLit(unsigned int instancesCount) : instancesCount(instancesCount), instancesBuffer(instancesCount, nullptr, 0, false, true)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PLANE_VERTICES_3D), PLANE_VERTICES_3D, GL_STATIC_DRAW);

    configured = false;
}

ParticlesLit::~ParticlesLit()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    instancesBuffer.~InstancesBuffer();
}

void ParticlesLit::Bind() const
{
    glBindVertexArray(VAO);
}

void ParticlesLit::Unbind() const
{
    glBindVertexArray(0);
}

BatchView ParticlesLit::AddProperties(float components, const void* data)
{
    unsigned int batchIndex = instancesBuffer.AddData(components, data);
    BatchView batchView{instancesBuffer.GetBatchPointer(batchIndex)};
    return batchView;
}

void ParticlesLit::ConfigureAttributes() const
{
    if (configured) return;

    Bind();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexParticlesLit), (void*)offsetof(VertexParticlesLit, texCoords));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexParticlesLit), (void*)offsetof(VertexParticlesLit, position));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexParticlesLit), (void*)offsetof(VertexParticlesLit, normal));
    instancesBuffer.ConfigureAttributes(3);
    Unbind();

    configured = true;
}

void ParticlesLit::Draw(unsigned int count) const
{
    if(!configured) {
        ConfigureAttributes();
    }

    if(count > instancesCount) {
        std::cerr << "Warning: Trying to draw more instances than the maximum. Drawing maximum instances instead." << std::endl;
        count = instancesCount;
    }

    if(count == 0) {
        count = instancesCount;
    }

    Bind();
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);
    Unbind();
}
