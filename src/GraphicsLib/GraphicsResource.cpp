#include "stdafx.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"


namespace GraphicsUtils {

void unique_texture::close() {
    glDeleteTextures(1, &resourceId_); LOGOPENGLERROR();
}

void unique_framebuffer::close() {
    glDeleteFramebuffers(1, &resourceId_); LOGOPENGLERROR();
}

void unique_renderbuffer::close() {
    glDeleteRenderbuffers(1, &resourceId_); LOGOPENGLERROR();
}

void unique_program::close() {
    glDeleteProgram(resourceId_); LOGOPENGLERROR();
}

void unique_vertex_array::close() {
    glDeleteVertexArrays(1, &resourceId_); LOGOPENGLERROR();
}

void unique_buffer::close() {
    glDeleteBuffers(1, &resourceId_); LOGOPENGLERROR();
}

} // namespace GraphicsUtils
