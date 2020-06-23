#include "stdafx.h"
#include "Config.h"
#include "Shader.h"
//#include "FreeType.h"
#include "Complex.h"
#include "Vector.h"
#include "FFT.h"
#include "Ocean.h"
#include "WorldPosition.h"
#include "FPSCounter.h"
#include "GlFormatter.h"

static const unsigned int Width  = 640;
static const unsigned int Height = 480;

static const char Title[] = "Ocean Simulation";

static const char ConfigFile[] = "./data/ocean.cfg";


/*****************************************************************************
 * Main variables
 ****************************************************************************/
bool gFullscreen;

int gWindowWidth, gWindowHeight;
float gScaleX, gScaleY;

FPSCounter gFPSCounter;

bool gShowHelp;
Position gPosition;

Ocean gOcean;
double gElapsedTime;
float gWindAmp;
Vector2 gWindDir;

glm::vec3 gLightPosition;
glm::mat4 gProjection, gView, gModel;

GEOMETRY_TYPE gGeometryType;

/*****************************************************************************
 * Graphics functions
 ****************************************************************************/
bool Init() {
    srand(time(0));

    LOGI << "OpenGL Renderer  : " << glGetString(GL_RENDERER);
    LOGI << "OpenGL Vendor    : " << glGetString(GL_VENDOR);
    LOGI << "OpenGL Version   : " << glGetString(GL_VERSION);
    LOGI << "GLSL Version     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);

    // Load config file
    Config config;
    config.Load(ConfigFile);
    LOGD << "Loaded Configuration File : " << ConfigFile;

    float windAmp;
    if (!config.Get("waveAmplitude", windAmp)) windAmp = 5e-4f;

    float windDirX, windDirZ;
    if (!config.Get("windDirX",      windDirX)) windDirX = 0.f;
    if (!config.Get("windDirZ",      windDirZ)) windDirZ = 32.f;

    int oceanRepeat, oceanSize;
    float oceanLen;
    if (!config.Get("oceanSize",   oceanSize))   oceanSize = 64;
    if (!config.Get("oceanLen",    oceanLen))    oceanLen = 64.f;
    if (!config.Get("oceanRepeat", oceanRepeat)) oceanRepeat = 10;

    // Ocean setup
    gGeometryType = GEOMETRY_SOLID;
    if (gOcean.init(oceanSize, windAmp, Vector2(windDirX, windDirZ),
            oceanLen, oceanRepeat) <= 0) {
        return false;
    }
    gOcean.geometryType(gGeometryType);

    // Other configurstions
    gFullscreen = false;
    gShowHelp = true;

    gWindowWidth = Width;
    gWindowHeight = Height;
    gScaleX = 2.f / (float)Width;
    gScaleY = 2.f / (float)Height;
    gPosition.resize_screen(Width, Height);

    gElapsedTime = 0.0;

    gProjection = glm::perspective(45.0f, (float)Width / (float)Height, 0.1f, 3000.0f);
    gView       = glm::mat4(1.0f);
    gModel      = glm::mat4(1.0f);

    gPosition.set_position(
        glm::vec3(0.0f, 100.0f, 0.0f), // Position
        glm::vec3(2.4f, -0.3f, 0.0f)); // Look angle

    gLightPosition = glm::vec3(gPosition.position.x + 1000.0, 100.0, gPosition.position.z - 1000.0);

    // Set up OpenGL flags
    // glClearColor(0.55f, 0.55f, 0.55f, 1.0f);
    glClearColor(0.25, 0.75, 0.65, 1.0);
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
    gFPSCounter.update(glfwGetTime());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gView = glm::lookAt(gPosition.position, gPosition.position + gPosition.lookat, gPosition.up);

    gOcean.geometryType(gGeometryType);
    gOcean.render(gElapsedTime, gLightPosition, gProjection, gView, gModel, true);
    
}

void Reshape(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
    gWindowWidth = width;
    gWindowHeight = height;
    gScaleX = 2.f / (float)width;
    gScaleY = 2.f / (float)height;
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
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0,
                    mode->width, mode->height, mode->refreshRate);
            }
            else {
                glfwSetWindowMonitor(window, nullptr, 0, 0,
                    Width, Height, GLFW_DONT_CARE);
            }
            break;

        case GLFW_KEY_F2:
            gShowHelp = !gShowHelp;
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
    static bool warp = false;

    if (!warp) {
        gPosition.set_mouse_point(x, y);
        glfwSetCursorPos(window, gWindowWidth / 2, gWindowHeight / 2);
        warp = true;
    } else {
        warp = false;
    }
}

void Update(GLFWwindow* window) {
    static double last_time = 0.0;
    double t = glfwGetTime();
    double dt = t - last_time;
    last_time = t;

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
    int status = EXIT_SUCCESS;
    
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

    LOGI << "Init window context with OpenGL 3.3 Core Profile";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    auto window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);
    if (!window) {
        LOGE << "Unable to Create OpenGL 3.3 Core Profile Context";
        status = EXIT_FAILURE;
        goto finish;
    }

    glfwSetKeyCallback(window, Keyboard);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwSetCursorPosCallback(window, MousePosition);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetWindowSizeCallback(window, Reshape);

    glfwMakeContextCurrent(window);
    gladLoadGL();

    if (!Init()) {
        LOGE << "Initialization failed";
        status = EXIT_FAILURE;
        goto finish;
    }

    while (!glfwWindowShouldClose(window)) {
        Display();

        Update(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

finish:
    Deinit();
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

    return status;
}
