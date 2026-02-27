#pragma once

#include <glad/gl.h>
#include <vector>
#include <iostream>

const unsigned int MULTISAMPLES = 4;

class Framebuffer
{
private:
    GLuint RBO;

    bool multisample;
    bool depthStencil;
    int colorAttachments;

    std::vector<GLuint> colorBuffers;

    unsigned int currentWidth;
    unsigned int currentHeight;
public:
    GLuint ID;
    Framebuffer(bool multisample, bool depthStencil, int colorAttachments, unsigned int width, unsigned int height);
    ~Framebuffer();

    void Bind() const;
    void Unbind() const;

    void BindTexture(int colorAttachment, GLenum textureUnit) const;

    void Resize(int width, int height);
    friend void BlitFramebuffer(Framebuffer &source, Framebuffer &destination, GLenum readBuffer, GLenum drawBuffer);
};