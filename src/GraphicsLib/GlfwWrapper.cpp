#include "stdafx.h"
#include "GlfwWrapper.h"


namespace GraphicsUtils {

GlfwWrapper::~GlfwWrapper() {
    Release();
}

int GlfwWrapper::Init(const std::string& title, int width, int height) {
    glfwSetErrorCallback(GlfwWrapper::ErrorCallback);

    if (glfwPlatformSupported(GLFW_PLATFORM_X11) &&
            glfwPlatformSupported(GLFW_PLATFORM_WAYLAND)) {
        // Prefer X11 instead of Wayland if both are available
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
        LOGI << "Use X11 under Wayland";
    }

    if (!glfwInit()) {
        LOGE << "Cannot load GLFW";
        return -1;
    }

#ifdef USE_OPENGL2_0
    LOGI << "Init window context with OpenGL 2.0";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
    LOGI << "Init window context with OpenGL 3.3";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        LOGE << "Cannot create OpenGL context";
        return -1;
    };

    glfwMakeContextCurrent(window_);
    gladLoadGL();

    return 0;
}

void GlfwWrapper::Release() {
    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    glfwTerminate();
}

GLFWwindow* GlfwWrapper::GetWindow() const {
    return window_;
}

void GlfwWrapper::ErrorCallback(int code, const char* description) {
    auto const codeStr = [code]() {
        switch (code) {
        case GLFW_NOT_INITIALIZED: return "NOT_INITIALIZED";
        case GLFW_NO_CURRENT_CONTEXT: return "NO_CURRENT_CONTEXT";
        case GLFW_INVALID_ENUM: return "INVALID_ENUM";
        case GLFW_INVALID_VALUE: return "INVALID_VALUE";
        case GLFW_OUT_OF_MEMORY: return "OUT_OF_MEMORY";
        case GLFW_API_UNAVAILABLE: return "API_UNAVAILABLE";
        case GLFW_VERSION_UNAVAILABLE: return "VERSION_UNAVAILABLE";
        case GLFW_PLATFORM_ERROR: return "PLATFORM_ERROR";
        case GLFW_FORMAT_UNAVAILABLE: return "FORMAT_UNAVAILABLE";
        case GLFW_NO_WINDOW_CONTEXT: return "NO_WINDOW_CONTEXT";
        default: return "Unknown error";
        }
    }();

    LOGE << "GLFW Error " << codeStr << " : " << description;
}

} // namespace FaceDetect
