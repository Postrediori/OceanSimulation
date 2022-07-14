#include "stdafx.h"
#include "GraphicsLogger.h"

void GraphicsUtils::LogOpenGlError(const char* file, int line) {
    GLenum err = glGetError();
    auto const errStr = [err]() {
        switch (err) {
        case GL_INVALID_ENUM: return "INVALID_ENUM";
        case GL_INVALID_VALUE: return "INVALID_VALUE";
        case GL_INVALID_OPERATION: return "INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case GL_STACK_UNDERFLOW: return "STACK_UNDERFLOW";
        case GL_STACK_OVERFLOW: return "STACK_OVERFLOW";
        default: return "Unknown error";
        }
    }();

    if (err != GL_NO_ERROR) {
        LOGE << " OpenGL Error in file " << file << " line " << line << " : " << errStr;
    }
}
