#pragma once

struct ScreenShaderInfo {
    const char *Name;
    const char *Vertex;
    const char *Fragment;
};

struct ScreenShader {
    ScreenShader() = default;

    int Init(ScreenShaderInfo info);

    void Render(GLuint texture);

    GraphicsUtils::unique_vertex_array vao;
    GraphicsUtils::unique_buffer quadVbo;

    GraphicsUtils::unique_program program;
    GLint uScreenTex{ -1 };
};
