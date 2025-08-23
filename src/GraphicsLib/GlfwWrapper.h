#pragma once

namespace GraphicsUtils {

    struct GlfwWrapper {
        GlfwWrapper() = default;
        ~GlfwWrapper();

        bool Init(const std::string& title, int width, int height);
        void Release();

        GLFWwindow* GetWindow() const;

        static void ErrorCallback(int error, const char* description);

        GLFWwindow* window_{ nullptr };

        int savedXPos_{ 0 }, savedYPos_{ 0 };
        int savedWidth_{ 0 }, savedHeight_{ 0 };
    };

}
