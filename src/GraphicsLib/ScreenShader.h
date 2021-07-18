#pragma once

struct ScreenShaderInfo {
    const char *Name;
    const char *Vertex;
    const char *Fragment;
};

struct ScreenShader {
    ~ScreenShader();

    int Init(ScreenShaderInfo info);
    void Release();

    void Render(GLuint texture);

    GLuint vao;
    GLuint quadVbo;

    GLuint program;
    GLint uScreenTex;
};
