#include "stdafx.h"
#include "Config.h"
#include "Shader.h"
#include "FreeType.h"
#include "Complex.h"
#include "Vector.h"
#include "FFT.h"
#include "Ocean.h"
#include "WorldPosition.h"
#include "FPSCounter.h"

static const unsigned int Width  = 640;
static const unsigned int Height = 480;

static const char Title[] = "Ocean Simulation";

static const char ConfigFile[] = "data/ocean.cfg";
static const char FontFile[] = "data/font.ttf";
static const FontSize_t FontSize = 24;

const GLfloat White[4] = {1.f, 1.f, 1.f, 1.f};

static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

/*****************************************************************************
 * Main variables
 ****************************************************************************/
bool gKeys[255];
bool gFullscreen;

int gWindowWidth, gWindowHeight;
float gScaleX, gScaleY;

FPSCounter gFPSCounter;
FontRenderer fr;
FontHandle_t a24;

bool gShowHelp;
Position gPosition;

Ocean gOcean;
double gElapsedTime;
float gWindAmp;
Vector2 gWindDir;

glm::vec3 gLightPosition;
glm::mat4 gProjection, gView, gModel;

GEOMETRY_TYPE gGeometryType;
static const char * const GeometryTypeNames[] = {
    "Wireframe",
    "Surface"
};

/*****************************************************************************
 * Graphics functions
 ****************************************************************************/
bool Init() {
    srand(time(0));

    LOGI << "OpenGL Renderer  : " << glGetString(GL_RENDERER);
    LOGI << "OpenGL Vendor    : " << glGetString(GL_VENDOR);
    LOGI << "OpenGL Version   : " << glGetString(GL_VERSION);
    LOGI << "GLSL Version     : " << glGetString(GL_SHADING_LANGUAGE_VERSION);
    LOGI << "FreeType Version : " << FREETYPE_MAJOR << "." << FREETYPE_MINOR << "." << FREETYPE_PATCH;

    // Load config file
    Config config;
    config.Load(ConfigFile);

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

    // Init FreeType
    if (!fr.init()) {
        return false;
    }
    if (!fr.load(FontFile)) {
        return false;
    }
    a24 = fr.createAtlas(FontSize);

    // Other configurstions
    gFullscreen = false;
    gShowHelp = true;
    gScaleX = 2.f / (float)Width;
    gScaleY = 2.f / (float)Height;

    gElapsedTime = 0.0;

    gProjection = glm::perspective(45.0f, (float)Width / (float)Height, 0.1f, 3000.0f);
    gView       = glm::mat4(1.0f);
    gModel      = glm::mat4(1.0f);

    gPosition.set_position(
        glm::vec3(0.0f, 100.0f, 0.0f) // Position
        , glm::vec3(2.4f, -0.3f, 0.0f)); // Look angle

    gLightPosition = glm::vec3(gPosition.position.x + 1000.0, 100.0, gPosition.position.z - 1000.0);

    // Set up OpenGL flags
    glShadeModel(GL_SMOOTH);

    // glClearColor(0.55f, 0.55f, 0.55f, 1.0f);
    glClearColor(0.25, 0.75, 0.65, 1.0);
    glClearDepth(1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    return true;
}

void Deinit() {
    fr.release();
}

/*****************************************************************************
 * GLUT Callback functions
 ****************************************************************************/
void Display() {
    gFPSCounter.update(glfwGetTime());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gView = glm::lookAt(gPosition.position, gPosition.position + gPosition.lookat, gPosition.up);

    gOcean.render(gElapsedTime, gLightPosition, gProjection, gView, gModel, true);

    if (gShowHelp) {
        fr.renderStart();
        fr.renderColor(White);

        //fr.renderText(a24, {-1+8*gScaleX, -1+225*gScaleY, gScaleX, gScaleY}, "1-2 ... Change rendering mode");
        //fr.renderText(a24, {-1+8*gScaleX, -1+200*gScaleY, gScaleX, gScaleY}, "Up/Down");
        //fr.renderText(a24, {-1+8*gScaleX, -1+175*gScaleY, gScaleX, gScaleY}, "Left/Right ... Move around");
        //fr.renderText(a24, {-1+8*gScaleX, -1+150*gScaleY, gScaleX, gScaleY}, "PgUp/PgDn");

        //fr.renderText(a24, {-1+8*gScaleX, -1+125*gScaleY, gScaleX, gScaleY}, "Mouse ... Look around");
        //fr.renderText(a24, {-1+8*gScaleX, -1+100*gScaleY, gScaleX, gScaleY}, "F2 ... Show/hide help");
        //fr.renderText(a24, {-1+8*gScaleX, -1+75*gScaleY, gScaleX, gScaleY}, "F1 ... Toggle fullscreen on/off");

        std::stringstream str;
        //str << "Mode : " << GeometryTypeNames[gGeometryType];
        //fr.renderText(a24, {-1+8*gScaleX, -1+50*gScaleY, gScaleX, gScaleY}, str.str());

        str.str(std::string());
        str << "FPS : " << std::setprecision(3) << gFPSCounter.fps;
        fr.renderText(a24, {-1+8*gScaleX, -1+25*gScaleY, gScaleX, gScaleY}, str.str());

        fr.renderEnd();
    }
}

void Error(int /*error*/, const char* description) {
    LOGE << "Error: " << description;
}

void Keyboard(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void Update() {
    static double last_time = 0.0;
    double t = glfwGetTime();
    double dt = t - last_time;
    last_time = t;

    gElapsedTime += dt * 0.5;

    gLightPosition = glm::vec3(gPosition.position.x + 1000.0, 100.0, gPosition.position.z - 1000.0);
}

/*****************************************************************************
 * Main program
 ****************************************************************************/
int main(int /*argc*/, char** /*argv*/) {
    plog::init(plog::debug, &consoleAppender);

    glfwSetErrorCallback(Error);

    if (!glfwInit()) {
        LOGE << "Failed to load GLFW";
        return 1;
    }

    LOGI << "Init window context with OpenGL 2.0";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    auto window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);
    if (!window) {
        LOGE << "Created OpenGL 2.0 Context";
        return 1;
    }

    glfwSetKeyCallback(window, Keyboard);

    glfwMakeContextCurrent(window);
    gladLoadGL();

    atexit(Deinit);
    if (!Init()) {
        LOGE << "Initialization failed";
        return 1;
    }

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        Display();

        Update();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
