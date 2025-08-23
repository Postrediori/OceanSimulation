#pragma once

namespace GraphicsUtils {

    struct ImGuiWrapper {
        ImGuiWrapper() = default;
        ~ImGuiWrapper();

        static void Init(GLFWwindow* window);
        static void Release();

        static void StartFrame();
        static void Render();
    };

}
