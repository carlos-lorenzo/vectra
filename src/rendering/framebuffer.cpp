#include "vectra/rendering/framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer(int width, int height)
{
    create(width, height);
}

Framebuffer::~Framebuffer()
{
    destroy();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : fbo_id_(other.fbo_id_),
      texture_id_(other.texture_id_),
      rbo_id_(other.rbo_id_),
      width_(other.width_),
      height_(other.height_)
{
    other.fbo_id_ = 0;
    other.texture_id_ = 0;
    other.rbo_id_ = 0;
    other.width_ = 0;
    other.height_ = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
{
    if (this != &other)
    {
        destroy();
        fbo_id_ = other.fbo_id_;
        texture_id_ = other.texture_id_;
        rbo_id_ = other.rbo_id_;
        width_ = other.width_;
        height_ = other.height_;

        other.fbo_id_ = 0;
        other.texture_id_ = 0;
        other.rbo_id_ = 0;
        other.width_ = 0;
        other.height_ = 0;
    }
    return *this;
}

void Framebuffer::create(int width, int height)
{
    width_ = width;
    height_ = height;

    // Create framebuffer
    glGenFramebuffers(1, &fbo_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);

    // Create color texture attachment
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);

    // Create depth/stencil renderbuffer
    glGenRenderbuffers(1, &rbo_id_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_id_);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::destroy()
{
    if (texture_id_ != 0)
    {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
    if (rbo_id_ != 0)
    {
        glDeleteRenderbuffers(1, &rbo_id_);
        rbo_id_ = 0;
    }
    if (fbo_id_ != 0)
    {
        glDeleteFramebuffers(1, &fbo_id_);
        fbo_id_ = 0;
    }
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id_);
    glViewport(0, 0, width_, height_);
}

void Framebuffer::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int width, int height)
{
    if (width == width_ && height == height_)
        return;

    if (width <= 0 || height <= 0)
        return;

    destroy();
    create(width, height);
}

