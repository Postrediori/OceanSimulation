#pragma once

namespace GraphicsUtils {

    struct ImGuiWrapper {
        ImGuiWrapper() = default;
        ~ImGuiWrapper();
        
        int Init(GLFWwindow* window);
        void Release();
        
        void StartFrame();
        void Render();
    };

}
