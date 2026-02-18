#include "Framebuffer.h"

Framebuffer::Framebuffer(bool multisample, bool depthStencil, int colorAttachments, unsigned int width, unsigned int height) : multisample(multisample), depthStencil(depthStencil), colorAttachments(colorAttachments), currentWidth(width), currentHeight(height)
{
    glGenFramebuffers(1, &ID);
    glBindFramebuffer(GL_FRAMEBUFFER, ID);

    colorBuffers.resize(colorAttachments);
    glGenTextures(colorAttachments, colorBuffers.data());
    for(int i = 0; i < colorAttachments; i++) {
        GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glBindTexture(target, colorBuffers[i]);
        if(multisample)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MULTISAMPLES, GL_RGB16F, width, height, GL_TRUE);
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, colorBuffers[i], 0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    if(depthStencil) {
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        if(multisample)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MULTISAMPLES, GL_DEPTH24_STENCIL8, width, height);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    }

    std::vector<GLenum> attachments;
    for (int i = 0; i < colorAttachments; i++)
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    glDrawBuffers(colorAttachments, attachments.data());

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &ID);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteTextures(colorAttachments, colorBuffers.data());
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void Framebuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::BindTexture(int colorAttachment, GLenum textureUnit) const
{
    glActiveTexture(textureUnit);
    GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glBindTexture(target, colorBuffers[colorAttachment]);
}

void Framebuffer::Resize(int width, int height)
{
    if(width == 0 || height == 0)
        return;
    
    currentWidth = width;
    currentHeight = height;

    for(int i = 0; i < colorAttachments; i++) {
        GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glBindTexture(target, colorBuffers[i]);
        if(multisample)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MULTISAMPLES, GL_RGB16F, width, height, GL_TRUE);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    if(depthStencil) {
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        if(multisample)
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, MULTISAMPLES, GL_DEPTH24_STENCIL8, width, height);
        else
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}

void BlitFramebuffer(Framebuffer &source, Framebuffer &destination, GLenum readBuffer, GLenum drawBuffer)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, source.ID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.ID);
    glReadBuffer(readBuffer);
    glDrawBuffer(drawBuffer);
    glBlitFramebuffer(0, 0, source.currentWidth, source.currentHeight, 0, 0, destination.currentWidth, destination.currentHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}