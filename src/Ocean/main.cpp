#include "stdafx.h"
#include "Config.h"
#include "Shader.h"
#include "Complex.h"
#include "Vector.h"
#include "FFT.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"
#include "GraphicsUtils.h"
#include "ResourceFinder.h"
#include "Benchmark.h"
#include "Ocean.h"
#include "WorldPosition.h"
#include "ScreenCapture.h"
#include "GlfwWrapper.h"
#include "ImGuiWrapper.h"
#ifndef USE_OPENGL2_0
#include "Framebuffer.h"
#include "ScreenShader.h"
#endif

constexpr int Width = 800;
constexpr int Height = 600;

const std::string Title = "Ocean Simulation";

const std::filesystem::path ConfigFile = "ocean.cfg";

constexpr double ElapsedTimeRate = 0.5;
constexpr double EvaluationTimeout = 1.0 / 32.0;

#ifndef USE_OPENGL2_0
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

struct WindowDimensions {
    int X{ 0 }, Y{ 0 };
    int Width{ 0 }, Height{ 0 };
};

struct OceanContext {
    OceanContext() = default;
    ~OceanContext() = default;

    bool Init(GLFWwindow* window, const std::string& modulePath);

    void Display();
    void DisplayUi();

    void Update();

    void RegisterCallbacks();

    void Reshape(int width, int height);
    void Keyboard(int key, int /*scancode*/, int action, int /*mods*/);
    void MousePosition(double x, double y);
    void MouseButtons(int button, int action, int mods);

    static void ReshapeCallback(GLFWwindow* window, int width, int height);
    static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MousePositionCallback(GLFWwindow* window, double x, double y);
    static void MouseButtonsCallback(GLFWwindow* window, int button, int action, int mods);

    GLFWwindow* window = nullptr;

    bool isFullscreen = false;
    WindowDimensions savedWindowPos;

    bool showUi = true;
    bool showColorsUi = false;

    int gWindowWidth = Width, gWindowHeight = Height;

    float fps = 0.0f;

    Position viewPosition;

    Ocean ocean;
    double elapsedTime = 0.0;
    float waveAmp = 2.0;
    float windDirX = 0.0f;
    float windDirZ = 32.0f;

    glm::vec3 lightPosition = glm::vec3();
    glm::mat4 projection = glm::mat4();
    glm::mat4 view = glm::mat4();
    glm::mat4 model = glm::mat4();

    GeometryRenderType geometryType = GeometryRenderType::Solid;

    ColorInfo fogColor{ 0.25, 0.75, 0.65, 1.0 };
    ColorInfo emissiveColor{ 1.0, 1.0, 1.0, 1.0 };
    ColorInfo ambientColor{ 0.0, 0.65, 0.75, 1.0 };
    ColorInfo diffuseColor{ 0.5, 0.65, 0.75, 1.0 };
    ColorInfo specularColor{ 1.0, 0.25, 0.0, 1.0 };

#ifndef USE_OPENGL2_0
    Framebuffer postProcFramebuffer;

    size_t currentScreenShader = 0;
    std::vector<ScreenShader> screenShaders;
#endif

    double lastTime{ 0.0 };
    double lastFpsTime{ 0.0 };
    double lastEvaluationTime{ 0.0 };

    Utils::OperationBenchmark evaluationBenchmark, renderingBenchmark;

    bool isEvaluating{ true };
    bool needsEvaluation{ true };
};

/*****************************************************************************
 * Graphics functions
 ****************************************************************************/
bool OceanContext::Init(GLFWwindow* w, const std::string& modulePath) {
    srand(time(0));

    window = w;

    auto dataDirOptional = Utils::ResourceFinder::GetDataDirectory(modulePath);
    if (!dataDirOptional) {
        LOGE << "Cannot find directory with data files";
        return false;
    }
    auto dataDir = *dataDirOptional;

    LOGI << "OpenGL Renderer  : " << glGetString(GL_RENDERER);
    LOGI << "OpenGL Vendor    : " << glGetString(GL_VENDOR);
    LOGI << "OpenGL Version   : " << glGetString(GL_VERSION);
    LOGI << "GLSL Version     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    LOGI << "GLM Version      : " << GLM_VERSION_MAJOR << "." << GLM_VERSION_MINOR << "." << GLM_VERSION_PATCH;
    LOGI << "GLFW Version     : " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION;
    LOGI << "ImGui Version    : " << IMGUI_VERSION << " (" << IMGUI_VERSION_NUM << ")";

    RegisterCallbacks();

    // Load config file
    auto configFilePath = dataDir / ConfigFile;
    Config config;
    config.Load(configFilePath.string());
    LOGD << "Loaded Configuration File : " << configFilePath.string();

    constexpr float DefaultWaveAmp = 2.0;
    constexpr float DefaultWindDirX = 0.0;
    constexpr float DefaultWindDirZ = 32.0;
    waveAmp = config.GetFloat("waveAmplitude").value_or(DefaultWaveAmp);
    windDirX = config.GetFloat("windDirX").value_or(DefaultWindDirX);
    windDirZ = config.GetFloat("windDirZ").value_or(DefaultWindDirZ);

    constexpr int DefaultOceanSize = 64;
    constexpr float DefaultOceanLen = 64.0;
    constexpr int DefaultOceanRepeat = 10;
    auto oceanSize = config.GetInt("oceanSize").value_or(DefaultOceanSize);
    auto oceanLen = config.GetFloat("oceanLen").value_or(DefaultOceanLen);
    auto oceanRepeat = config.GetInt("oceanRepeat").value_or(DefaultOceanRepeat);

    // Ocean setup
    geometryType = GeometryRenderType::Solid;
    if (!ocean.init(dataDir, oceanSize, waveAmp, Vector2(windDirX, windDirZ),
            oceanLen, oceanRepeat)) {
        return false;
    }
    ocean.geometryType(geometryType);

#ifndef USE_OPENGL2_0
    // Screen shader and framebuffer
    for (const auto& info : ScreenShadersInfo) {
        ScreenShaderInfo i = { info.Name, dataDir / info.Vertex, dataDir / info.Fragment };

        ScreenShader s;
        if (!s.Init(i)) {
            LOGE << "Failed to load screen shader";
            continue;
        }
        screenShaders.push_back(std::move(s));
    }

    if (!postProcFramebuffer.Init(Width, Height)) {
        LOGE << "Failed to create framebuffer";
        return false;
    }
    postProcFramebuffer.Resize(Width, Height);
#endif

    // Other configurstions
    projection = glm::perspective(45.0f, (float)Width / (float)Height, 0.1f, 3000.0f);
    view = glm::mat4(1.0f);
    model = glm::mat4(1.0f);

    viewPosition.SetPosition(
        glm::vec3(0.0f, 100.0f, 0.0f), // Position
        glm::vec3(2.4f, -0.3f, 0.0f)); // Look angle

    lightPosition = glm::vec3(viewPosition.position.x + 1000.0, 100.0, viewPosition.position.z - 1000.0);

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
    renderingBenchmark.StartOperation();

#ifndef USE_OPENGL2_0
    // Start using framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, postProcFramebuffer.GetFramebuffer()); LOGOPENGLERROR();
#endif

    // Render scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); LOGOPENGLERROR();

    view = glm::lookAt(viewPosition.position, viewPosition.position + viewPosition.lookat, viewPosition.up);

    glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]); LOGOPENGLERROR();

    ocean.geometryType(geometryType);
    ocean.colorFog(fogColor);
    ocean.colorEmissive(emissiveColor);
    ocean.colorAmbient(ambientColor);
    ocean.colorDiffuse(diffuseColor);
    ocean.colorSpecular(specularColor);
    ocean.render(lightPosition, projection, view, model);

#ifndef USE_OPENGL2_0
    // Finish using framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0); LOGOPENGLERROR();

    // Render post-processed image
    // With Mesa3d Depth bit should also be cleaned even if the 'scene' there is a 2D plane
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); LOGOPENGLERROR();

    screenShaders[currentScreenShader].Render(postProcFramebuffer.GetTexture(),
        postProcFramebuffer.GetWidth(), postProcFramebuffer.GetHeight());
#endif

    if (showUi) {
        // Render ImGui window
        DisplayUi();
    }

    renderingBenchmark.EndOperation();
}

void OceanContext::DisplayUi() {
    constexpr float UiMargin = 10.0f;
#ifdef USE_OPENGL2_0
    static const ImVec2 UiSize = ImVec2(300, 330);
#else
    static const ImVec2 UiSize = ImVec2(300, 385);
#endif

    ImGui::SetNextWindowPos(ImVec2(UiMargin, gWindowHeight - UiSize.y - UiMargin), ImGuiCond_Always);
    ImGui::SetNextWindowSize(UiSize, ImGuiCond_Always);

    ImGui::Begin("Ocean Simulation", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::Text("Rendering mode:");
    ImGui::RadioButton("Wireframe", reinterpret_cast<int *>(&geometryType), static_cast<int>(GeometryRenderType::Wireframe)); ImGui::SameLine();
    ImGui::RadioButton("Solid", reinterpret_cast<int *>(&geometryType), static_cast<int>(GeometryRenderType::Solid));

    ImGui::Separator();

    ImGui::Text("Ocean parameters:");
    if (ImGui::SliderFloat("Choppiness", &waveAmp, 0.0f, 5.0f, "%.1f")) {
        ocean.windAmp(waveAmp);
        needsEvaluation = true;
    }
    if (ImGui::SliderFloat("Wind", &windDirZ, 0.0f, 50.0f, "%.1f m/s")) {
        ocean.windDirZ(windDirZ);
        needsEvaluation = true;
    }

    ImGui::Separator();

    if (ImGui::Button("Show/Hide Colors >")) {
        showColorsUi = !showColorsUi;
    }

    ImGui::Separator();

#ifndef USE_OPENGL2_0
    ImGui::Text("Post-processing shader:");
    const auto& elemName = ScreenShadersInfo[currentScreenShader].Name;
    int k = static_cast<int>(currentScreenShader);
    if (ImGui::SliderInt("##", &k, 0, ScreenShadersInfo.size() - 1, elemName.c_str())) {
        currentScreenShader = static_cast<int>(k);
    }

    ImGui::Separator();
#endif

    ImGui::Text("User Guide:");
    ImGui::BulletText("F1 to on/off fullscreen mode.");
    ImGui::BulletText("F2 to show/hide UI.");
    ImGui::BulletText("F11 to save screenshot to file.");
    ImGui::BulletText("1/2 to change rendering mode.");
    ImGui::BulletText("K to start/stop simulation.");
#ifndef USE_OPENGL2_0
    ImGui::BulletText("S to switch post-processing filter.");
#endif
    ImGui::BulletText("Arrow keys/PgUp/PgDown to navigate.");
    ImGui::BulletText("RMB and move mouse to rotate view.");
    ImGui::BulletText("ESCAPE to exit.");

    ImGui::Separator();

    ImGui::Text("FPS Counter: %.1f", fps);

    ImGui::End();

    if (showColorsUi) {
        static const ImVec2 ColorsUiSize = ImVec2(300, 130);

        ImGui::SetNextWindowPos(ImVec2(UiSize.x + UiMargin * 2,
            gWindowHeight - ColorsUiSize.y - UiMargin), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ColorsUiSize, ImGuiCond_Always);

        ImGui::Begin("Colors parameters", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        if (ImGui::ColorEdit3("Fog", fogColor.data())) {
            ocean.colorFog(fogColor);
        }
        if (ImGui::ColorEdit3("Emissive", emissiveColor.data())) {
            ocean.colorEmissive(emissiveColor);
        }
        if (ImGui::ColorEdit3("Ambient", ambientColor.data())) {
            ocean.colorAmbient(ambientColor);
        }
        if (ImGui::ColorEdit3("Diffuse", diffuseColor.data())) {
            ocean.colorDiffuse(diffuseColor);
        }
        if (ImGui::ColorEdit3("Specular", specularColor.data())) {
            ocean.colorSpecular(specularColor);
        }

        ImGui::End();
    }
}

void OceanContext::Reshape(int width, int height) {
    glViewport(0, 0, width, height);
    gWindowWidth = width;
    gWindowHeight = height;
#ifndef USE_OPENGL2_0
    postProcFramebuffer.Resize(width, height);
#endif
}

void OceanContext::Keyboard(int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;

        case GLFW_KEY_F1:
            isFullscreen = !isFullscreen;
            if (isFullscreen) {
                glfwGetWindowPos(window, &savedWindowPos.X, &savedWindowPos.Y);
                glfwGetWindowSize(window, &savedWindowPos.Width, &savedWindowPos.Height);

                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0,
                    mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr,
                    savedWindowPos.X, savedWindowPos.Y,
                    savedWindowPos.Width, savedWindowPos.Height, GLFW_DONT_CARE);
            }
            break;

        case GLFW_KEY_F2:
            showUi = !showUi;
            break;

        case GLFW_KEY_F11:
            ScreenCapture::SaveToFile(ScreenCapture::Format::Png, gWindowWidth, gWindowHeight);
            break;

        case GLFW_KEY_1:
            geometryType = GeometryRenderType::Wireframe;
            break;

        case GLFW_KEY_2:
            geometryType = GeometryRenderType::Solid;
            break;

        case GLFW_KEY_K:
            isEvaluating = !isEvaluating;
            break;

#ifndef USE_OPENGL2_0
        case GLFW_KEY_S:
            currentScreenShader = (currentScreenShader + 1) % screenShaders.size();
            break;
#endif
        }
    }
}

void OceanContext::MousePosition(double x, double y) {
    viewPosition.MouseMove(x, y);
}

void OceanContext::MouseButtons(int button, int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_2) {
        switch (action) {
        case GLFW_PRESS: {
                double x = 0.0, y = 0.0;
                glfwGetCursorPos(window, &x, &y);
                viewPosition.MouseDown(x, y);
            }
            break;

        case GLFW_RELEASE:
            viewPosition.MouseUp();
            break;

        default:
            break;
        }
    }
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

void OceanContext::MouseButtonsCallback(GLFWwindow* window, int button, int action, int mods) {
    auto p = glfwGetWindowUserPointer(window);
    assert(p);

    auto pContext = reinterpret_cast<OceanContext*>(p);
    pContext->MouseButtons(button, action, mods);
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
    glfwSetMouseButtonCallback(window, OceanContext::MouseButtonsCallback);

    glfwSetWindowSizeCallback(window, OceanContext::ReshapeCallback);
}

void OceanContext::Update() {
    double currentTime = glfwGetTime();
    double dt = currentTime - lastTime;
    lastTime = currentTime;

    if (isEvaluating) {
        elapsedTime += dt * ElapsedTimeRate;

        if (currentTime - lastEvaluationTime > EvaluationTimeout) {
            needsEvaluation = true;
        }
    }

    if (needsEvaluation) {
        evaluationBenchmark.StartOperation();
        ocean.evaluate(elapsedTime, true);
        evaluationBenchmark.EndOperation();
        lastEvaluationTime = currentTime;
        needsEvaluation = false;
    }

    if (currentTime - lastFpsTime > 1.0) {
        fps = ImGui::GetIO().Framerate;

        LOGI << "Average FPS: " << static_cast<int>(fps) <<
            ", Evaluation (us): " << evaluationBenchmark.GetAverage() <<
            ", Rendering (us): " << renderingBenchmark.GetAverage();

        evaluationBenchmark.Reset();
        renderingBenchmark.Reset();

        lastFpsTime = currentTime;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        viewPosition.MoveForward(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        viewPosition.MoveBack(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        viewPosition.MoveLeft(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        viewPosition.MoveRight(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
        viewPosition.MoveUp(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
        viewPosition.MoveDown(dt);
    }

    lightPosition = glm::vec3(viewPosition.position.x + 1000.0,
        100.0, viewPosition.position.z - 1000.0);
}

/*****************************************************************************
 * Main program
 ****************************************************************************/
int main(int /*argc*/, const char* argv[]) {
    try {
        plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
#ifdef NDEBUG
        plog::init(plog::info, &consoleAppender);
#else
        plog::init(plog::debug, &consoleAppender);
#endif

        GraphicsUtils::GlfwWrapper glfwWrapper;
        if (!glfwWrapper.Init(Title, Width, Height)) {
            LOGE << "Failed to load GLFW";
            return EXIT_FAILURE;
        }

        glfwSwapInterval(0); // Disable vsync to get maximum number of iterations

        OceanContext context;
        if (!context.Init(glfwWrapper.GetWindow(), argv[0])) {
            LOGE << "Initialization failed";
            return EXIT_FAILURE;
        }

        // Setup ImGui
        GraphicsUtils::ImGuiWrapper imguiWrapper;
        imguiWrapper.Init(glfwWrapper.GetWindow());

        // Setup of ImGui visual style
        ImGui::StyleColorsClassic();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.WindowBorderSize = 0.0f;

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
