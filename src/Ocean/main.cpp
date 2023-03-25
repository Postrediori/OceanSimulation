#include "stdafx.h"
#include "Config.h"
#include "Shader.h"
#include "Complex.h"
#include "Vector.h"
#include "FFT.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"
#include "ResourceFinder.h"
#include "Ocean.h"
#include "WorldPosition.h"
#include "LogFormatter.h"
#include "ScreenCapture.h"
#include "Framebuffer.h"
#include "GlfwWrapper.h"
#include "ImGuiWrapper.h"
#include "ScreenShader.h"

constexpr int Width = 800;
constexpr int Height = 600;

const std::string Title = "Ocean Simulation";

const std::filesystem::path ConfigFile = "ocean.cfg";

#ifdef USE_OPENGL2_0
const std::vector<ScreenShaderInfo> ScreenShadersInfo = {
    {"Normal", "screen110.vert", "screen110-normal.frag"},
    {"Gray", "screen110.vert", "screen110-gray.frag"},
    {"Blur", "screen110.vert", "screen110-blur.frag"},
    {"Sobel", "screen110.vert", "screen110-sobel.frag"},
    {"Dither B&W", "screen110.vert", "screen110-dither.frag"},
    {"Dither GB", "screen110.vert", "screen110-dither-gb.frag"}
};
#else
const std::vector<ScreenShaderInfo> ScreenShadersInfo = {
    {"Normal", "screen.vert", "screen-normal.frag"},
    {"Gray", "screen.vert", "screen-gray.frag"},
    {"Blur", "screen.vert", "screen-blur.frag"},
    {"Sobel", "screen.vert", "screen-sobel.frag"},
    {"Dither B&W", "screen.vert", "screen-dither.frag"},
    {"Dither GB", "screen.vert", "screen-dither-gb.frag"}
};
#endif


/*****************************************************************************
 * Main variables
 ****************************************************************************/
struct OceanContext {
    OceanContext() = default;

    bool Init(GLFWwindow* window, const std::string& modulePath);

    void Display();
    void DisplayUi();

    void Update();

    void RegisterCallbacks();

    void Reshape(int width, int height);
    void Keyboard(int key, int /*scancode*/, int action, int /*mods*/);
    void MousePosition(double x, double y);

    static void ReshapeCallback(GLFWwindow* window, int width, int height);
    static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MousePositionCallback(GLFWwindow* window, double x, double y);

    GLFWwindow* window = nullptr;

    bool gFullscreen = false;
    struct {
        int XPos, YPos;
        int Width, Height;
    } gSavedWindowPos = { 0, 0, 0, 0 };

    bool gShowUi = true;
    bool gShowColorsUi = false;

    int gWindowWidth = Width, gWindowHeight = Height;

    float gFps = 0.0f;

    Position gPosition;

    Ocean gOcean;
    double gElapsedTime = 0.0;
    float gWaveAmp = 2.0e-5f;
    float gWindDirX = 0.0f;
    float gWindDirZ = 32.0f;

    glm::vec3 gLightPosition = glm::vec3();
    glm::mat4 gProjection = glm::mat4();
    glm::mat4 gView = glm::mat4();
    glm::mat4 gModel = glm::mat4();

    GeometryRenderType gGeometryType = GeometryRenderType::Solid;

    ImVec4 gFogColor = ImVec4(0.25, 0.75, 0.65, 1.0);
    ImVec4 gEmissiveColor = ImVec4(1.0, 1.0, 1.0, 1.0);
    ImVec4 gAmbientColor = ImVec4(0.0, 0.65, 0.75, 1.0);
    ImVec4 gDiffuseColor = ImVec4(0.5, 0.65, 0.75, 1.0);
    ImVec4 gSpecularColor = ImVec4(1.0, 0.25, 0.0, 1.0);

    Framebuffer gFramebuffer;

    int gCurrentScreenShader = 0;
    std::vector<ScreenShader> gScreenShaders;
};

/*****************************************************************************
 * Graphics functions
 ****************************************************************************/
bool OceanContext::Init(GLFWwindow* w, const std::string& modulePath) {
    srand(time(0));

    window = w;

    std::filesystem::path dataDir;
    if (!Utils::ResourceFinder::GetDataDirectory(modulePath, dataDir)) {
        LOGE << "Cannot find directory with data files";
        return false;
    }

    LOGI << "OpenGL Renderer  : " << glGetString(GL_RENDERER);
    LOGI << "OpenGL Vendor    : " << glGetString(GL_VENDOR);
    LOGI << "OpenGL Version   : " << glGetString(GL_VERSION);
    LOGI << "GLSL Version     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    RegisterCallbacks();

    // Load config file
    auto configFilePath = dataDir / ConfigFile;
    Config config;
    config.Load(configFilePath.string());
    LOGD << "Loaded Configuration File : " << configFilePath.string();

    config.Get("waveAmplitude", gWaveAmp);
    config.Get("windDirX", gWindDirX);
    config.Get("windDirZ", gWindDirZ);

    int oceanRepeat, oceanSize;
    float oceanLen;
    if (!config.Get("oceanSize", oceanSize)) oceanSize = 64;
    if (!config.Get("oceanLen", oceanLen)) oceanLen = 64.f;
    if (!config.Get("oceanRepeat", oceanRepeat)) oceanRepeat = 10;

    // Ocean setup
    gGeometryType = GeometryRenderType::Solid;
    if (gOcean.init(dataDir, oceanSize, gWaveAmp, Vector2(gWindDirX, gWindDirZ),
            oceanLen, oceanRepeat) <= 0) {
        return false;
    }
    gOcean.geometryType(gGeometryType);

    // Screen shader and framebuffer
    for (const auto& info : ScreenShadersInfo) {
        ScreenShaderInfo i = { info.Name, dataDir / info.Vertex, dataDir / info.Fragment };

        ScreenShader s;
        if (!s.Init(i)) {
            LOGE << "Failed to load screen shader";
            continue;
        }
        gScreenShaders.push_back(std::move(s));
    }

    if (!gFramebuffer.Init(Width, Height)) {
        LOGE << "Failed to create framebuffer";
        return false;
    }
    gFramebuffer.Resize(Width, Height);

    // Other configurstions
    gPosition.resize_screen(Width, Height);

    gProjection = glm::perspective(45.0f, (float)Width / (float)Height, 0.1f, 3000.0f);
    gView = glm::mat4(1.0f);
    gModel = glm::mat4(1.0f);

    gPosition.set_position(
        glm::vec3(0.0f, 100.0f, 0.0f), // Position
        glm::vec3(2.4f, -0.3f, 0.0f)); // Look angle

    gLightPosition = glm::vec3(gPosition.position.x + 1000.0, 100.0, gPosition.position.z - 1000.0);

    // Set up OpenGL flags
    glClearDepth(1.0); LOGOPENGLERROR();
    glClearStencil(0); LOGOPENGLERROR();

    glEnable(GL_DEPTH_TEST); LOGOPENGLERROR();
    glDepthFunc(GL_LEQUAL); LOGOPENGLERROR();

    return true;
}

/*****************************************************************************
 * GLUT Callback functions
 ****************************************************************************/
void OceanContext::Display() {
    // Start using framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, gFramebuffer.GetFramebuffer()); LOGOPENGLERROR();

    // Render scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); LOGOPENGLERROR();

    gView = glm::lookAt(gPosition.position, gPosition.position + gPosition.lookat, gPosition.up);

    glClearColor(gFogColor.x, gFogColor.y, gFogColor.z, gFogColor.w); LOGOPENGLERROR();

    gOcean.geometryType(gGeometryType);
    gOcean.colors((float*)&gFogColor, (float*)&gEmissiveColor,
        (float*)&gAmbientColor, (float*)&gDiffuseColor, (float*)&gSpecularColor);
    gOcean.render(gElapsedTime, gLightPosition, gProjection, gView, gModel, true);

    // Finish using framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0); LOGOPENGLERROR();

    // Render post-processed image
    glClear(GL_COLOR_BUFFER_BIT); LOGOPENGLERROR();

    gScreenShaders[gCurrentScreenShader].Render(gFramebuffer.GetTexture(),
        gFramebuffer.GetWidth(), gFramebuffer.GetHeight());

    if (gShowUi) {
        // Render ImGui window
        DisplayUi();
    }
}

void OceanContext::DisplayUi() {
    constexpr float UiMargin = 10.0f;
    static const ImVec2 UiSize = ImVec2(300, 345);

    ImGui::SetNextWindowPos(ImVec2(UiMargin, gWindowHeight - UiSize.y - UiMargin), ImGuiCond_Always);
    ImGui::SetNextWindowSize(UiSize, ImGuiCond_Always);

    ImGui::Begin("Ocean Simulation", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::Text("Rendering mode:");
    ImGui::RadioButton("Wireframe", (int *)&gGeometryType, static_cast<int>(GeometryRenderType::Wireframe)); ImGui::SameLine();
    ImGui::RadioButton("Solid", (int *)&gGeometryType, static_cast<int>(GeometryRenderType::Solid));

    ImGui::Separator();

    ImGui::Text("Ocean parameters:");
    static float waveAmp = gWaveAmp * 1e5;
    if (ImGui::SliderFloat("Choppiness", &waveAmp, 0.0f, 5.0f, "%.1f")) {
        gWaveAmp = waveAmp * 1e-5;
        gOcean.windAmp(gWaveAmp);
    }
    if (ImGui::SliderFloat("Wind", &gWindDirZ, 0.0f, 50.0f, "%.1f m/s")) {
        gOcean.windDirZ(gWindDirZ);
    }

    ImGui::Separator();

    if (ImGui::Button("Show/Hide Colors >")) {
        gShowColorsUi = !gShowColorsUi;
    }

    ImGui::Separator();

    ImGui::Text("Post-processing shader:");
    const auto& elemName = ScreenShadersInfo[gCurrentScreenShader].Name;
    ImGui::SliderInt("##", &gCurrentScreenShader, 0, ScreenShadersInfo.size() - 1, elemName.c_str());

    ImGui::Separator();

    ImGui::Text("User Guide:");
    ImGui::BulletText("F1 to on/off fullscreen mode.");
    ImGui::BulletText("F2 to show/hide UI.");
    ImGui::BulletText("F11 to save screenshot to file.");
    ImGui::BulletText("1/2 to change rendering mode.");
    ImGui::BulletText("Arrow keys/PgUp/PgDown to navigate.");
    ImGui::BulletText("ESCAPE to exit.");

    ImGui::Separator();

    ImGui::Text("FPS Counter: %.1f", gFps);

    ImGui::End();

    if (gShowColorsUi) {
        static const ImVec2 ColorsUiSize = ImVec2(300, 130);

        ImGui::SetNextWindowPos(ImVec2(UiSize.x + UiMargin * 2,
            gWindowHeight - ColorsUiSize.y - UiMargin), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ColorsUiSize, ImGuiCond_Always);

        ImGui::Begin("Colors parameters", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        ImGui::ColorEdit3("Fog", (float*)&gFogColor);
        ImGui::ColorEdit3("Emissive", (float*)&gEmissiveColor);
        ImGui::ColorEdit3("Ambient", (float*)&gAmbientColor);
        ImGui::ColorEdit3("Diffuse", (float*)&gDiffuseColor);
        ImGui::ColorEdit3("Specular", (float*)&gSpecularColor);

        ImGui::End();
    }
}

void OceanContext::Reshape(int width, int height) {
    glViewport(0, 0, width, height);
    gWindowWidth = width;
    gWindowHeight = height;
    gPosition.resize_screen(width, height);
    gFramebuffer.Resize(width, height);
}

void OceanContext::Keyboard(int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;

        case GLFW_KEY_F1:
            gFullscreen = !gFullscreen;
            if (gFullscreen) {
                glfwGetWindowPos(window, &gSavedWindowPos.XPos, &gSavedWindowPos.YPos);
                glfwGetWindowSize(window, &gSavedWindowPos.Width, &gSavedWindowPos.Height);

                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0,
                    mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr,
                    gSavedWindowPos.XPos, gSavedWindowPos.YPos,
                    gSavedWindowPos.Width, gSavedWindowPos.Height, GLFW_DONT_CARE);
            }
            break;

        case GLFW_KEY_F2:
            gShowUi = !gShowUi;
            break;

        case GLFW_KEY_F11:
            ScreenCapture::SaveToFile(ScreenCapture::Format::Png, gWindowWidth, gWindowHeight);
            break;

        case GLFW_KEY_1:
            gGeometryType = GeometryRenderType::Wireframe;
            break;

        case GLFW_KEY_2:
            gGeometryType = GeometryRenderType::Solid;
            break;

        case GLFW_KEY_S:
            gCurrentScreenShader++;
            if (gCurrentScreenShader>= gScreenShaders.size()) {
                gCurrentScreenShader = 0;
            }
            break;
        }
    }
}

void OceanContext::MousePosition(double x, double y) {
    //static bool warp = false;

    //if (!warp) {
    //    gPosition.set_mouse_point(x, y);
    //    glfwSetCursorPos(window, gWindowWidth / 2, gWindowHeight / 2);
    //    warp = true;
    //} else {
    //    warp = false;
    //}
}

void OceanContext::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto p = glfwGetWindowUserPointer(window);
    assert(p);

    auto pContext = reinterpret_cast<OceanContext*>(p);
    pContext->Keyboard(key, scancode, action, mods);
}

void OceanContext::MousePositionCallback(GLFWwindow* window, double x, double y) {
    auto p = glfwGetWindowUserPointer(window);
    assert(p);

    auto pContext = reinterpret_cast<OceanContext*>(p);
    pContext->MousePosition(x, y);
}

void OceanContext::ReshapeCallback(GLFWwindow* window, int width, int height) {
    auto p = glfwGetWindowUserPointer(window);
    assert(p);

    auto pContext = reinterpret_cast<OceanContext*>(p);
    pContext->Reshape(width, height);
}

void OceanContext::RegisterCallbacks() {
    glfwSetWindowUserPointer(window, static_cast<void*>(this));

    glfwSetKeyCallback(window, OceanContext::KeyboardCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetCursorPosCallback(window, OceanContext::MousePositionCallback);

    glfwSetWindowSizeCallback(window, OceanContext::ReshapeCallback);
}

void OceanContext::Update() {
    static double lastTime = 0.0;
    static double lastFpsTime = 0.0;
    double currentTime = glfwGetTime();
    double dt = currentTime - lastTime;
    lastTime = currentTime;

    if (currentTime - lastFpsTime > 1.0) {
        gFps = ImGui::GetIO().Framerate;
        lastFpsTime = currentTime;
    }

    gElapsedTime += dt * 0.5;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        gPosition.move_forward(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        gPosition.move_back(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        gPosition.move_left(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        gPosition.move_right(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        gPosition.move_up(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        gPosition.move_down(dt);
    }

    gLightPosition = glm::vec3(gPosition.position.x + 1000.0,
        100.0, gPosition.position.z - 1000.0);
}

/*****************************************************************************
 * Main program
 ****************************************************************************/
int main(int argc, const char* argv[]) {
    try {
        plog::ConsoleAppender<plog::LogFormatter> consoleAppender;
#ifdef NDEBUG
        plog::init(plog::info, &consoleAppender);
#else
        plog::init(plog::debug, &consoleAppender);
#endif

        GraphicsUtils::GlfwWrapper glfwWrapper;
        if (glfwWrapper.Init(Title, Width, Height) != 0) {
            LOGE << "Failed to load GLFW";
            return EXIT_FAILURE;
        }

        glfwSwapInterval(0); // Disable vsync to get maximum number of iterations

        // Setup ImGui
        GraphicsUtils::ImGuiWrapper imguiWrapper;
        imguiWrapper.Init(glfwWrapper.GetWindow());

        // Setup of ImGui visual style
        ImGui::StyleColorsClassic();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.WindowBorderSize = 0.0f;

        OceanContext context;
        if (!context.Init(glfwWrapper.GetWindow(), argv[0])) {
            LOGE << "Initialization failed";
            return EXIT_FAILURE;
        }

        while (!glfwWindowShouldClose(glfwWrapper.GetWindow())) {
            glfwPollEvents();

            // Start ImGui frame
            imguiWrapper.StartFrame();

            // Render objects
            context.Display();

            // Render ImGui
            imguiWrapper.Render();

            // Update objects
            context.Update();

            glfwMakeContextCurrent(glfwWrapper.GetWindow());
            glfwSwapBuffers(glfwWrapper.GetWindow());
        }
    }
    catch (const std::exception& ex) {
        LOGE << "Exception " << ex.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
