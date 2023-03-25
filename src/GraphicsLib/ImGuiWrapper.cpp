#include "stdafx.h"
#include "ImGuiWrapper.h"

namespace GraphicsUtils {

ImGuiWrapper::~ImGuiWrapper() {
    Release();
}

void ImGuiWrapper::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable .ini

#ifdef USE_OPENGL2_0
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL2_Init();
#else
    static const std::string ImguiGlslVersion = "#version 330 core";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(ImguiGlslVersion.c_str());
#endif
}

void ImGuiWrapper::Release() {
#ifdef USE_OPENGL2_0
    ImGui_ImplOpenGL2_Shutdown();
#else
    ImGui_ImplOpenGL3_Shutdown();
#endif
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiWrapper::StartFrame() {
#ifdef USE_OPENGL2_0
    ImGui_ImplOpenGL2_NewFrame();
#else
    ImGui_ImplOpenGL3_NewFrame();
#endif
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiWrapper::Render() {
    ImGui::Render();
#ifdef USE_OPENGL2_0
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#else
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}

} // namespace GraphicsUtils
