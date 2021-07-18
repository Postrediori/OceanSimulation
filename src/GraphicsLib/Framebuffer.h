#pragma once

struct Framebuffer {
    ~Framebuffer();

    int Init(int w, int h);
    int Resize(int w, int h);
    void Release();

    int width{0}, height{0};
    GLuint frame_buffer{0};
    GLuint tex_color_buffer{0};
    GLuint rbo_depth_stencil{0};
};
