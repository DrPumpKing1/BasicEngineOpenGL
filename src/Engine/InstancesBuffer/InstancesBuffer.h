#pragma once

#include <glad/gl.h>
#include <vector>
#include <iostream>

struct Batch {
    unsigned int components;
    size_t offset;
    const void* data;
};

struct BatchView {
    size_t size;
    float *data;
};

class InstancesBuffer 
{
private:
    GLuint VBO;

    const unsigned int instancesCount;

    std::vector<Batch> batchData;
    size_t size = 0;

    mutable bool configured = false;

    bool resizeable = false;
    bool dynamic = false;
    bool calculateSize = false;

    mutable float *bufferPointer = nullptr;
public:
    InstancesBuffer(unsigned int instancesCount, const void *data = nullptr, size_t maxSize = 0, bool resizeable = false, bool dynamic = false);
    ~InstancesBuffer();

    void Bind() const;
    void Unbind() const;

    void UpdateData(unsigned int offset, unsigned int size, const void* data) const;

    unsigned int AddData(unsigned int components, const void* data);
    void ConfigureAttributes(unsigned int firstAttributeIndex) const;

    BatchView GetBatchPointer(unsigned int batchIndex, unsigned int firstInstance = 0, unsigned int instances = 0);
private:
    void AllocateBuffer() const;
};