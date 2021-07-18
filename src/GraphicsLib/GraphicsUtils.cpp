#include "stdafx.h"
#include "GraphicsUtils.h"

const std::unordered_map<GLenum, std::string> OpenGlErrors = {
        {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
        {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
        {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
        {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"},
        {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
    };

void GraphicsUtils::LogOpenGlError(const char* file, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE << " OpenGL Error in file " << file
            << " line " << line << " : "
            << OpenGlErrors.at(err);
    }
}
