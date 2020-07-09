#include "stdafx.h"
#include "Config.h"
#include "Shader.h"
#include "Complex.h"
#include "Vector.h"
#include "FFT.h"
#include "Ocean.h"
#include "WorldPosition.h"
#include "GlFormatter.h"
#include "ScopeGuard.h"
#include "ScreenCapture.h"

static const unsigned int Width  = 800;
static const unsigned int Height = 600;

static const char Title[] = "Ocean Simulation";

static const char ConfigFile[] = "./data/ocean.cfg";


/*****************************************************************************
 * Main variables
 ****************************************************************************/
bool gFullscreen = false;
bool gShowUi = true;

int gWindowWidth = Width, gWindowHeight = Height;

int gSavedXPos = 0, gSavedYPos = 0;
int gSavedWidth = 0, gSavedHeight = 0;

float gFps = 0.0f;

Position gPosition;

Ocean gOcean;
double gElapsedTime = 0.0;
float gWaveAmp = 2.0e-5f;
float gWindDirX = 0.0f;
float gWindDirZ = 32.0f;

glm::vec3 gLightPosition;
glm::mat4 gProjection, gView, gModel;

GEOMETRY_TYPE gGeometryType;

ImVec4 gFogColor = ImVec4(0.25, 0.75, 0.65, 1.0);
ImVec4 gEmissiveColor = ImVec4(1.0, 1.0, 1.0, 1.0);
ImVec4 gAmbientColor = ImVec4(0.0, 0.65, 0.75, 1.0);
ImVec4 gDiffuseColor = ImVec4(0.5, 0.65, 0.75, 1.0);
ImVec4 gSpecularColor = ImVec4(1.0, 0.25, 0.0, 1.0);


/*****************************************************************************
 * Graphics functions
 ****************************************************************************/
bool Init() {
    srand(time(0));

    LOGI << "OpenGL Renderer  : " << glGetString(GL_RENDERER);
    LOGI << "OpenGL Vendor    : " << glGetString(GL_VENDOR);
    LOGI << "OpenGL Version   : " << glGetString(GL_VERSION);
    LOGI << "GLSL Version     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    // Setup of ImGui visual style
    ImGui::StyleColorsClassic();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 0.0f;

    // Load config file
    Config config;
    config.Load(ConfigFile);
    LOGD << "Loaded Configuration File : " << ConfigFile;

    config.Get("waveAmplitude", gWaveAmp);
    config.Get("windDirX", gWindDirX);
    config.Get("windDirZ", gWindDirZ);

    int oceanRepeat, oceanSize;
    float oceanLen;
    if (!config.Get("oceanSize",   oceanSize))   oceanSize = 64;
    if (!config.Get("oceanLen",    oceanLen))    oceanLen = 64.f;
    if (!config.Get("oceanRepeat", oceanRepeat)) oceanRepeat = 10;

    // Ocean setup
    gGeometryType = GEOMETRY_SOLID;
    if (gOcean.init(oceanSize, gWaveAmp, Vector2(gWindDirX, gWindDirZ),
            oceanLen, oceanRepeat) <= 0) {
        return false;
    }
    gOcean.geometryType(gGeometryType);

    // Other configurstions
    gPosition.resize_screen(Width, Height);

    gProjection = glm::perspective(45.0f, (float)Width / (float)Height, 0.1f, 3000.0f);
    gView       = glm::mat4(1.0f);
    gModel      = glm::mat4(1.0f);

    gPosition.set_position(
        glm::vec3(0.0f, 100.0f, 0.0f), // Position
        glm::vec3(2.4f, -0.3f, 0.0f)); // Look angle

    gLightPosition = glm::vec3(gPosition.position.x + 1000.0, 100.0, gPosition.position.z - 1000.0);

    // Set up OpenGL flags
    glClearDepth(1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    return true;
}

void Deinit() {
    gOcean.release();
}

void Error(int /*error*/, const char* description) {
    LOGE << "Error: " << description;
}

/*****************************************************************************
 * GLUT Callback functions
 ****************************************************************************/
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gView = glm::lookAt(gPosition.position, gPosition.position + gPosition.lookat, gPosition.up);

    glClearColor(gFogColor.x, gFogColor.y, gFogColor.z, gFogColor.w);

    gOcean.geometryType(gGeometryType);
    gOcean.colors((float*)&gFogColor, (float*)&gEmissiveColor,
        (float*)&gAmbientColor, (float*)&gDiffuseColor, (float*)&gSpecularColor);
    gOcean.render(gElapsedTime, gLightPosition, gProjection, gView, gModel, true);
}

void DisplayUi() {
    static const float UiMargin = 10.0f;
    static const ImVec2 UiSize = ImVec2(300, 295);

    ImGui::SetNextWindowPos(ImVec2(UiMargin, gWindowHeight - UiSize.y - UiMargin), ImGuiCond_Always);
    ImGui::SetNextWindowSize(UiSize, ImGuiCond_Always);

    ImGui::Begin("Ocean Simulation", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::Text("Rendering mode:");
    ImGui::RadioButton("Wireframe", (int *)&gGeometryType, (int)GEOMETRY_TYPE::GEOMETRY_LINES); ImGui::SameLine();
    ImGui::RadioButton("Solid", (int *)&gGeometryType, (int)GEOMETRY_TYPE::GEOMETRY_SOLID);

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

    static bool showColorsUi = false;
    if (ImGui::Button("Show/Hide Colors >")) {
        showColorsUi = !showColorsUi;
    }

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

    if (showColorsUi) {
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

void Reshape(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
    gWindowWidth = width;
    gWindowHeight = height;
    gPosition.resize_screen(width, height);
}

void Keyboard(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;

        case GLFW_KEY_F1:
            gFullscreen = !gFullscreen;
            if (gFullscreen) {
                glfwGetWindowPos(window, &gSavedXPos, &gSavedYPos);
                glfwGetWindowSize(window, &gSavedWidth, &gSavedHeight);

                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0,
                    mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr, gSavedXPos, gSavedYPos,
                    gSavedWidth, gSavedHeight, GLFW_DONT_CARE);
            }
            break;

        case GLFW_KEY_F2:
            gShowUi = !gShowUi;
            break;

        case GLFW_KEY_F11:
            ScreenCapture::SaveToFile(ScreenCaptureFormat::Png, gWindowWidth, gWindowHeight);
            break;

        case GLFW_KEY_1:
            gGeometryType = GEOMETRY_LINES;
            break;

        case GLFW_KEY_2:
            gGeometryType = GEOMETRY_SOLID;
            break;
        }
    }
}

void MousePosition(GLFWwindow* window, double x, double y) {
    //static bool warp = false;

    //if (!warp) {
    //    gPosition.set_mouse_point(x, y);
    //    glfwSetCursorPos(window, gWindowWidth / 2, gWindowHeight / 2);
    //    warp = true;
    //} else {
    //    warp = false;
    //}
}

void Update(GLFWwindow* window) {
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
int main(int /*argc*/, char** /*argv*/) {
    try {

        static plog::ConsoleAppender<plog::GlFormatter> consoleAppender;
#ifdef NDEBUG
        plog::init(plog::info, &consoleAppender);
#else
        plog::init(plog::debug, &consoleAppender);
#endif

        glfwSetErrorCallback(Error);

        if (!glfwInit()) {
            LOGE << "Failed to load GLFW";
            return EXIT_FAILURE;
        }
        ScopeGuard glfwGuard([]() {
            glfwTerminate();
            LOGD << "Cleanup : GLFW context";
        });

        LOGI << "Init window context with OpenGL 3.3 Core Profile";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        auto window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);
        if (!window) {
            LOGE << "Unable to Create OpenGL 3.3 Core Profile Context";
            return EXIT_FAILURE;
        }
        ScopeGuard windowGuard([window]() {
            glfwDestroyWindow(window);
            LOGD << "Cleanup : GLFW window";
        });

        glfwSetKeyCallback(window, Keyboard);
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

        glfwSetCursorPosCallback(window, MousePosition);

        glfwSetWindowSizeCallback(window, Reshape);

        glfwMakeContextCurrent(window);
        gladLoadGL();

        // Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr; // Disable .ini

        static const char* gGlslVersion = "#version 330 core";
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(gGlslVersion);

        ScopeGuard imGuiContextGuard([]() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            LOGD << "Cleanup : ImGui";
        });

        if (!Init()) {
            LOGE << "Initialization failed";
            return EXIT_FAILURE;
        }
        ScopeGuard scopeGuard([]() {
            Deinit();
            LOGD << "Cleanup : Simulation";
        });

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render objects
            Display();

            // Render ImGui window
            if (gShowUi) {
                DisplayUi();
            }

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            // Update objects
            Update(window);

            glfwSwapBuffers(window);
        }

    }
    catch (const std::exception& ex) {
        LOGE << "Exception " << ex.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
