#include "stdafx.h"
#include "GraphicsUtils.h"
#include "Framebuffer.h"

Framebuffer::~Framebuffer() {
    Release();
}

int Framebuffer::Init(int w, int h) {
    width = w;
    height = h;

    // Create color texture buffer
    glGenTextures(1, &tex_color_buffer); LOGOPENGLERROR();
    if (!tex_color_buffer) {
        LOGE << "Failed to create texture for framebuffer";
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, tex_color_buffer); LOGOPENGLERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); LOGOPENGLERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); LOGOPENGLERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr); LOGOPENGLERROR();

    // glBindTexture(GL_TEXTURE_2D, 0); LOGOPENGLERROR();

    // Create render buffer object
    glGenRenderbuffers(1, &rbo_depth_stencil); LOGOPENGLERROR();
    if (!rbo_depth_stencil) {
        LOGE << "Failed to create render buffer object";
        return 0;
    }

    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth_stencil); LOGOPENGLERROR();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h); LOGOPENGLERROR();

    // Create framebuffer
    glGenFramebuffers(1, &frame_buffer); LOGOPENGLERROR();
    if (!frame_buffer) {
        LOGE << "Failed to create framebuffer object";
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer); LOGOPENGLERROR();

    // Bind texture and render buffer to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           tex_color_buffer, 0); LOGOPENGLERROR();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_depth_stencil); LOGOPENGLERROR();

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER); LOGOPENGLERROR();
    if (fboStatus!=GL_FRAMEBUFFER_COMPLETE) {
        LOGE << "Error: Framebuffer Error " << fboStatus;
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); LOGOPENGLERROR();

    return 1;
}

int Framebuffer::Resize(int w, int h) {
    Release();
    return Init(w, h);
}

void Framebuffer::Release() {
    if (rbo_depth_stencil) {
        glDeleteRenderbuffers(1, &rbo_depth_stencil); LOGOPENGLERROR();
        rbo_depth_stencil = 0;
    }
    if (tex_color_buffer) {
        glDeleteTextures(1, &tex_color_buffer); LOGOPENGLERROR();
        tex_color_buffer = 0;
    }
    if (frame_buffer) {
        glDeleteFramebuffers(1, &frame_buffer); LOGOPENGLERROR();
        frame_buffer = 0;
    }
}
