#pragma once

#ifdef NDEBUG
# define LOGOPENGLERROR()
#else
# define LOGOPENGLERROR() GraphicsUtils::LogOpenGlError(__FILE__,__LINE__)
#endif

namespace GraphicsUtils
{
    void LogOpenGlError(const char *file, int line);
}
