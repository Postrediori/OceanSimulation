#include "stdafx.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"
#include "Framebuffer.h"

int Framebuffer::Init(int w, int h) {
    width = w;
    height = h;

    // Create color texture buffer
    glGenTextures(1, tex_color_buffer.put()); LOGOPENGLERROR();
    if (!tex_color_buffer) {
        LOGE << "Failed to create texture for framebuffer";
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, tex_color_buffer.get()); LOGOPENGLERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); LOGOPENGLERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); LOGOPENGLERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr); LOGOPENGLERROR();

    // Create render buffer object
    glGenRenderbuffers(1, rbo_depth_stencil.put()); LOGOPENGLERROR();
    if (!rbo_depth_stencil) {
        LOGE << "Failed to create render buffer object";
        return 0;
    }

    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth_stencil.get()); LOGOPENGLERROR();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); LOGOPENGLERROR();

    // Create framebuffer
    glGenFramebuffers(1, frame_buffer.put()); LOGOPENGLERROR();
    if (!frame_buffer) {
        LOGE << "Failed to create framebuffer object";
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer.get()); LOGOPENGLERROR();

    // Bind texture and render buffer to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           tex_color_buffer.get(), 0); LOGOPENGLERROR();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rbo_depth_stencil.get()); LOGOPENGLERROR();

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER); LOGOPENGLERROR();
    if (fboStatus!=GL_FRAMEBUFFER_COMPLETE) {
        auto const errStr = [fboStatus]() {
            switch (fboStatus) {
            case GL_FRAMEBUFFER_UNDEFINED: return "FRAMEBUFFER_UNDEFINED";
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            case GL_FRAMEBUFFER_UNSUPPORTED: return "FRAMEBUFFER_UNSUPPORTED";
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return "FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            default: return "Unknown error";
            }
        }();

        LOGE << "Error: Framebuffer Error " << errStr;
        return 0;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); LOGOPENGLERROR();
    glBindRenderbuffer(GL_RENDERBUFFER, 0); LOGOPENGLERROR();

    return 1;
}

int Framebuffer::Resize(int w, int h) {
    Release();
    return Init(w, h);
}

void Framebuffer::Release() {
    tex_color_buffer.reset();
    rbo_depth_stencil.reset();
    frame_buffer.reset();
}
