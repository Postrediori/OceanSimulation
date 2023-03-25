#include "stdafx.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"
#include "Shader.h"
#include "ScreenShader.h"

constexpr size_t QuadVerticesCount = 6;
const std::vector<GLfloat> QuadVertices = {
    -1., 1., 0., 1.,
    1., 1., 1., 1.,
    1., -1., 1., 0.,

    1., -1., 1., 0.,
    -1., -1., 0., 0.,
    -1., 1., 0., 1.
};

int ScreenShader::Init(const ScreenShaderInfo& info) {
    // Init shader
    program.reset(Shader::CreateProgramFromFiles(info.Vertex.string(), info.Fragment.string()));
    if (!program) {
        LOGE << "Failed to create program for screen shader";
        return 0;
    }

    uScreenTex = glGetUniformLocation(program.get(), "tex"); LOGOPENGLERROR();
#ifdef USE_OPENGL2_0
    uTexSize = glGetUniformLocation(program.get(), "tex_size"); LOGOPENGLERROR();
#endif

#ifndef USE_OPENGL2_0
    // Init vertex array
    glGenVertexArrays(1, vao.put()); LOGOPENGLERROR();
    if (!vao) {
        LOGE << "Failed to create VAO for screen shader";
        return 0;
    }

    glBindVertexArray(vao.get()); LOGOPENGLERROR();
#endif

    // Init quad vertex buffer
    glGenBuffers(1, quadVbo.put()); LOGOPENGLERROR();
    if (!quadVbo) {
        LOGE << "Failed to create VBO for screen shader";
        return 0;
    }

    glBindBuffer(GL_ARRAY_BUFFER, quadVbo.get()); LOGOPENGLERROR();
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * QuadVertices.size(), QuadVertices.data(),
        GL_STATIC_DRAW); LOGOPENGLERROR();

    aScreenCoord = glGetAttribLocation(program.get(), "coord"); LOGOPENGLERROR();
    aScreenTexCoord = glGetAttribLocation(program.get(), "tex_coord"); LOGOPENGLERROR();

    InitBufferAttributes();

#ifndef USE_OPENGL2_0
    glBindVertexArray(0); LOGOPENGLERROR();
#endif

    return 1;
}

void ScreenShader::Render(GLuint texture, int w, int h) {
    glUseProgram(program.get()); LOGOPENGLERROR();

#ifdef USE_OPENGL2_0
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo.get()); LOGOPENGLERROR();
    InitBufferAttributes();
    glUniform2i(uTexSize, w, h); LOGOPENGLERROR();
#else
    glBindVertexArray(vao.get()); LOGOPENGLERROR();
#endif

    glBindTexture(GL_TEXTURE_2D, texture); LOGOPENGLERROR();

    glActiveTexture(GL_TEXTURE0); LOGOPENGLERROR();
    glUniform1i(uScreenTex, 0); LOGOPENGLERROR();

    // Init screen quad draw
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo.get()); LOGOPENGLERROR();

    // Screen quad draw
    glDrawArrays(GL_TRIANGLES, 0, QuadVerticesCount); LOGOPENGLERROR();

    glUseProgram(0); LOGOPENGLERROR();
#ifndef USE_OPENGL2_0
    glBindVertexArray(0); LOGOPENGLERROR();
#endif
}

void ScreenShader::InitBufferAttributes() {
    glEnableVertexAttribArray(aScreenCoord); LOGOPENGLERROR();
    glVertexAttribPointer(aScreenCoord, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(GLfloat), reinterpret_cast<void*>(0)); LOGOPENGLERROR();

    glEnableVertexAttribArray(aScreenTexCoord); LOGOPENGLERROR();
    glVertexAttribPointer(aScreenTexCoord, 2, GL_FLOAT, GL_FALSE,
        4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat))); LOGOPENGLERROR();
}
