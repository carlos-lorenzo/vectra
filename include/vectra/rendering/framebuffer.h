#ifndef VECTRA_FRAMEBUFFER_H
#define VECTRA_FRAMEBUFFER_H

#include <glad/glad.h>

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    // Prevent copying
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    // Allow moving
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;

    void bind() const;
    static void unbind();

    void resize(int width, int height);

    [[nodiscard]] GLuint get_texture_id() const { return texture_id_; }
    [[nodiscard]] int get_width() const { return width_; }
    [[nodiscard]] int get_height() const { return height_; }

private:
    void create(int width, int height);
    void destroy();

    GLuint fbo_id_ = 0;
    GLuint texture_id_ = 0;
    GLuint rbo_id_ = 0;  // Depth/stencil renderbuffer
    int width_ = 0;
    int height_ = 0;
};

#endif //VECTRA_FRAMEBUFFER_H

