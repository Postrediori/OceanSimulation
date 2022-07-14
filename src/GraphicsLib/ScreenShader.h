#pragma once

struct ScreenShaderInfo {
    const char *Name;
    const char *Vertex;
    const char *Fragment;
};

struct ScreenShader {
    ScreenShader() = default;
    ScreenShader(ScreenShader&& other)
        : vao(std::move(other.vao))
        , quadVbo(std::move(other.quadVbo))
        , program(std::move(other.program)) {
    }

    int Init(ScreenShaderInfo info);

    void Render(GLuint texture);

    GraphicsUtils::unique_vertex_array vao;
    GraphicsUtils::unique_buffer quadVbo;

    GraphicsUtils::unique_program program;
    GLint uScreenTex{ -1 };
};
