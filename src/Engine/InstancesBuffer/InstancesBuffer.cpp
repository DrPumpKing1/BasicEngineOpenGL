#include "InstancesBuffer.h"

InstancesBuffer::InstancesBuffer(unsigned int instancesCount, const void *data, size_t maxSize, bool resizeable, bool dynamic) : instancesCount(instancesCount), size(maxSize), resizeable(resizeable), dynamic(dynamic)
{
    glGenBuffers(1, &VBO);
    calculateSize = maxSize == 0;

    Bind();
    if(!calculateSize)
    {
        if (resizeable)
        {
            glBufferData(GL_ARRAY_BUFFER, size, data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
        else
        {
            GLenum flags = (dynamic ? GL_MAP_WRITE_BIT : 0) | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            glBufferStorage(GL_ARRAY_BUFFER, size, data, flags);
        }
        configured = true;
    }

    if(dynamic && !resizeable && !calculateSize) {
        bufferPointer = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
        if(bufferPointer == nullptr) {
            std::cerr << "Error: Failed to map buffer pointer." << std::endl;
        }
    }
    Unbind();
}

InstancesBuffer::~InstancesBuffer()
{
    batchData.clear();
    if(bufferPointer != nullptr) {
        Bind();
        glUnmapBuffer(GL_ARRAY_BUFFER);
        Unbind();
        bufferPointer = nullptr;
    }
    glDeleteBuffers(1, &VBO);
}

void InstancesBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void InstancesBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancesBuffer::UpdateData(unsigned int offset, unsigned int size, const void* data) const
{
    if(!dynamic){
        std::cerr << "Error: Buffer is not dynamic. Cannot update data." << std::endl;
        return;
    }

    AllocateBuffer();
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    Unbind();
}

unsigned int InstancesBuffer::AddData(unsigned int components, const void* data)
{
    if(configured && !resizeable) {
        return -1;
    }

    size_t dataSize = components * instancesCount * sizeof(float);
    size_t offset = size;
    batchData.push_back({components, offset, data});
    size += dataSize;

    configured = false;

    return batchData.size() - 1;
}

void InstancesBuffer::ConfigureAttributes(unsigned int firstAttributeIndex) const
{
    AllocateBuffer();
    Bind();
    size_t offset = 0;
    for (size_t i = 0; i < batchData.size(); i++)
    {
        Batch batch = batchData[i];

        glEnableVertexAttribArray(firstAttributeIndex + i);
        glVertexAttribPointer(firstAttributeIndex + i, batch.components, GL_FLOAT, GL_FALSE, batch.components * sizeof(float), (void*)offset);
        glVertexAttribDivisor(firstAttributeIndex + i, 1);
        offset += batch.components * instancesCount * sizeof(float);
    }
    Unbind();
}

void InstancesBuffer::AllocateBuffer() const
{
    if (configured || !calculateSize) return;

    if(size == 0) {
        std::cerr << "Error: Buffer has not declared an schema to reallocate." << std::endl;
    }

    Bind();
    if (resizeable)
    {
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    }
    else
    {
        GLenum flags = (dynamic ? GL_MAP_WRITE_BIT : 0) | GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glBufferStorage(GL_ARRAY_BUFFER, size, nullptr, flags);
    }

    for (const Batch &batch : batchData)
    {
        size_t dataSize = batch.components * instancesCount * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, batch.offset, dataSize, batch.data);
        std::cout << "Error code: " << glGetError() << std::endl;
    }

    if(dynamic && !resizeable) {
        bufferPointer = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT));
        if(bufferPointer == nullptr) {
            std::cerr << "Error: Failed to map buffer pointer." << std::endl;
        }
    }

    Unbind();
    configured = true;
}

BatchView InstancesBuffer::GetBatchPointer(unsigned int batchIndex, unsigned int firstInstance, unsigned int instances)
{
    if(!dynamic) {
        std::cerr << "Error: Buffer is not dynamic. Cannot get batch pointer." << std::endl;
        return BatchView{0, nullptr};
    }

    if(resizeable)
    {
        std::cerr << "Error: Buffer is resizeable. Cannot get batch pointer." << std::endl;
        return BatchView{0, nullptr};
    }

    AllocateBuffer();

    if(batchIndex >= batchData.size()) {
        std::cerr << "Error: Batch index out of range." << std::endl;
        return BatchView{0, nullptr};
    }
    
    if(instances == 0) {
        instances = instancesCount - firstInstance;
    }

    if(firstInstance + instances > instancesCount) {
        std::cerr << "Error: First instance bigger than instances count." << std::endl;
        return BatchView{0, nullptr};
    }

    const Batch& batch = batchData[batchIndex];
    size_t instanceSize = batch.components * sizeof(float);
    size_t offset = batch.offset + (firstInstance * instanceSize);
    size_t size = instances * instanceSize;
    size_t batchEnd = batch.offset + (instancesCount * instanceSize);

    if (offset + size > batchEnd) {
        size = batchEnd - offset;
    }

    if(bufferPointer == nullptr) {
        std::cerr << "Error: Buffer is not mapped. Cannot get batch pointer." << std::endl;
        return BatchView{0, nullptr};
    }

    float* ptr = bufferPointer + (offset / sizeof(float));

    return BatchView{
        size,
        ptr
    };
}