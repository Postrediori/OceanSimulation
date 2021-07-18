#include "stdafx.h"
#include "GraphicsUtils.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "ScreenShader.h"

constexpr size_t QuadVerticesCount = 6;
const std::array<GLfloat,4*QuadVerticesCount> QuadVertices = {
    -1., 1., 0., 1.,
    1., 1., 1., 1.,
    1., -1., 1., 0.,

    1., -1., 1., 0.,
    -1., -1., 0., 0.,
    -1., 1., 0., 1.
};

ScreenShader::~ScreenShader() {
    Release();
}

int ScreenShader::Init(ScreenShaderInfo info) {
    // Init shader
    program = Shader::CreateProgram(info.Vertex, info.Fragment);
    if (program == 0) {
        LOGE << "Failed to create program for screen shader";
        return 0;
    }

    uScreenTex = glGetUniformLocation(program, "tex"); LOGOPENGLERROR();

    // Init vertex array
    glGenVertexArrays(1, &vao); LOGOPENGLERROR();
    if (vao == 0) {
        LOGE << "Failed to create VAO for screen shader";
        return 0;
    }

    glBindVertexArray(vao); LOGOPENGLERROR();

    // Init quad vertex buffer
    glGenBuffers(1, &quadVbo); LOGOPENGLERROR();
    if (quadVbo == 0) {
        LOGE << "Failed to create VBO for screen shader";
        return 0;
    }

    glBindBuffer(GL_ARRAY_BUFFER, quadVbo); LOGOPENGLERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertices), QuadVertices.data(), GL_STATIC_DRAW); LOGOPENGLERROR();

    GLint aScreenCoord    = glGetAttribLocation(program, "coord"); LOGOPENGLERROR();
    GLint aScreenTexCoord = glGetAttribLocation(program, "tex_coord"); LOGOPENGLERROR();

    glEnableVertexAttribArray(aScreenCoord); LOGOPENGLERROR();
    glVertexAttribPointer(aScreenCoord, 2, GL_FLOAT, GL_FALSE,
                          4*sizeof(GLfloat), reinterpret_cast<void*>(0)); LOGOPENGLERROR();

    glEnableVertexAttribArray(aScreenTexCoord); LOGOPENGLERROR();
    glVertexAttribPointer(aScreenTexCoord, 2, GL_FLOAT, GL_FALSE,
                          4*sizeof(GLfloat), reinterpret_cast<void *>(2*sizeof(GLfloat))); LOGOPENGLERROR();

    glBindVertexArray(0); LOGOPENGLERROR();

    return 1;
}

void ScreenShader::Release() {
    if (vao) {
        glDeleteVertexArrays(1, &vao); LOGOPENGLERROR();
        vao = 0;
    }
    if (quadVbo) {
        glDeleteBuffers(1, &quadVbo); LOGOPENGLERROR();
        quadVbo = 0;
    }
    if (program) {
        glDeleteProgram(program); LOGOPENGLERROR();
        program = 0;
    }
}

void ScreenShader::Render(GLuint texture) {
    glUseProgram(program); LOGOPENGLERROR();
    glBindVertexArray(vao); LOGOPENGLERROR();

    glBindTexture(GL_TEXTURE_2D, texture); LOGOPENGLERROR();

    glActiveTexture(GL_TEXTURE0); LOGOPENGLERROR();
    glUniform1i(uScreenTex, 0); LOGOPENGLERROR();

    // Init screen quad draw
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo); LOGOPENGLERROR();

    // Screen quad draw
    glDrawArrays(GL_TRIANGLES, 0, QuadVerticesCount); LOGOPENGLERROR();

    glUseProgram(0); LOGOPENGLERROR();
    glBindVertexArray(0); LOGOPENGLERROR();
}
