#pragma once

namespace GraphicsUtils {

    struct ImGuiWrapper {
        ImGuiWrapper() = default;
        ~ImGuiWrapper();
        
        void Init(GLFWwindow* window);
        void Release();
        
        void StartFrame();
        void Render();
    };

}
