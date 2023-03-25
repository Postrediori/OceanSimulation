#pragma once

struct ScreenShaderInfo {
    std::string Name;
    std::filesystem::path Vertex;
    std::filesystem::path Fragment;
};

struct ScreenShader {
    ScreenShader() = default;
#ifdef USE_OPENGL2_0
    ScreenShader(ScreenShader&& other) noexcept
        : quadVbo(std::move(other.quadVbo))
        , program(std::move(other.program))
        , uScreenTex(other.uScreenTex), uTexSize(other.uTexSize)
        , aScreenCoord(other.aScreenCoord), aScreenTexCoord(other.aScreenTexCoord) { }
#else
    ScreenShader(ScreenShader&& other) noexcept
        : vao(std::move(other.vao))
        , quadVbo(std::move(other.quadVbo))
        , program(std::move(other.program))
        , uScreenTex(other.uScreenTex)
        , aScreenCoord(other.aScreenCoord), aScreenTexCoord(other.aScreenTexCoord) { }
#endif

    int Init(const ScreenShaderInfo& info);

    void Render(GLuint texture, int w, int h);

    void InitBufferAttributes();

#ifndef USE_OPENGL2_0
    GraphicsUtils::unique_vertex_array vao;
#endif
    GraphicsUtils::unique_buffer quadVbo;

    GraphicsUtils::unique_program program;
    GLint uScreenTex = -1;
#ifdef USE_OPENGL2_0
    // Shader needs resolution as uniform because GLSL 1.10 doesn't have textureSize
    GLint uTexSize = -1;
#endif
    GLint aScreenCoord = -1, aScreenTexCoord = -1;
};
