#pragma once

struct Framebuffer {
    Framebuffer() = default;

    int Init(int w, int h);
    int Resize(int w, int h);
    void Release();

    GLuint GetFramebuffer() const { return frame_buffer.get(); }
    GLuint GetTexture() const { return tex_color_buffer.get(); }

    int width{0}, height{0};
    GraphicsUtils::unique_framebuffer frame_buffer;
    GraphicsUtils::unique_texture tex_color_buffer;
    GraphicsUtils::unique_renderbuffer rbo_depth_stencil;
};
