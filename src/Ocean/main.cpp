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

    std::cout<<"OpenGL Renderer  : "<<glGetString(GL_RENDERER)<<std::endl;
    std::cout<<"OpenGL Vendor    : "<<glGetString(GL_VENDOR)<<std::endl;
    std::cout<<"OpenGL Version   : "<<glGetString(GL_VERSION)<<std::endl;
    std::cout<<"GLSL Version     : "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;
    std::cout<<"GLEW Version     : "<<glewGetString(GLEW_VERSION)<<std::endl;
    std::cout<<"FreeType Version : "<<FREETYPE_MAJOR<<"."<<FREETYPE_MINOR<<"."<<FREETYPE_PATCH<<std::endl;

    // Init GLEW
    glewExperimental = GL_TRUE;

    GLenum err = glewInit();
    if (err!=GLEW_OK) {
        std::cerr << "GL Loading Error: " << glewGetErrorString(err) << std::endl;
        return false;
    }

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
    gScaleX = 2.f / (float)gWindowWidth;
    gScaleY = 2.f / (float)gWindowHeight;

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
    gFPSCounter.update(glutGet(GLUT_ELAPSED_TIME));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gView = glm::lookAt(gPosition.position, gPosition.position + gPosition.lookat, gPosition.up);

    gOcean.render(gElapsedTime, gLightPosition, gProjection, gView, gModel, true);

    if (gShowHelp) {
        fr.renderStart();
        fr.renderColor(White);

        fr.renderText(a24, {-1+8*gScaleX, -1+225*gScaleY, gScaleX, gScaleY}, "1-2 ... Change rendering mode");
        fr.renderText(a24, {-1+8*gScaleX, -1+200*gScaleY, gScaleX, gScaleY}, "Up/Down");
        fr.renderText(a24, {-1+8*gScaleX, -1+175*gScaleY, gScaleX, gScaleY}, "Left/Right ... Move around");
        fr.renderText(a24, {-1+8*gScaleX, -1+150*gScaleY, gScaleX, gScaleY}, "PgUp/PgDn");

        fr.renderText(a24, {-1+8*gScaleX, -1+125*gScaleY, gScaleX, gScaleY}, "Mouse ... Look around");
        fr.renderText(a24, {-1+8*gScaleX, -1+100*gScaleY, gScaleX, gScaleY}, "F2 ... Show/hide help");
        fr.renderText(a24, {-1+8*gScaleX, -1+75*gScaleY, gScaleX, gScaleY}, "F1 ... Toggle fullscreen on/off");

        std::stringstream str;
        str << "Mode : " << GeometryTypeNames[gGeometryType];
        fr.renderText(a24, {-1+8*gScaleX, -1+50*gScaleY, gScaleX, gScaleY}, str.str());

        str.str(std::string());
        str << "FPS : " << std::setprecision(3) << gFPSCounter.fps;
        fr.renderText(a24, {-1+8*gScaleX, -1+25*gScaleY, gScaleX, gScaleY}, str.str());

        fr.renderEnd();
    }

    glutSwapBuffers();
}

void Reshape(GLint w, GLint h) {
    glViewport(0, 0, w, h);
    gWindowWidth  = w;
    gWindowHeight = h;

    gScaleX = 2.f / (float)gWindowWidth;
    gScaleY = 2.f / (float)gWindowHeight;

    glutWarpPointer(w/2, h/2);
    gProjection = glm::perspective(45.0f, (float)w / (float)h, 0.1f, 3000.0f);

    gPosition.resize_screen(w, h);
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 27:
        exit(0);
        break;

    case '1':
    case '2':
        gGeometryType = GEOMETRY_TYPE((int)key - (int)'0' - 1);
        gOcean.geometryType(gGeometryType);
        break;
    }
}

void SpecialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        gFullscreen = !gFullscreen;
        if (gFullscreen) {
            glutFullScreen();
        } else {
            glutReshapeWindow(Width, Height);
        }
        break;

    case GLUT_KEY_F2:
        gShowHelp = !gShowHelp;
        break;

    default:
        gKeys[key] = true;
    }
}

void SpecialKeysUp(int key, int x, int y) {
    gKeys[key] = false;
}

void Idle() {
    static int last_time = 0;
    int t = glutGet(GLUT_ELAPSED_TIME);
    float dt = (t - last_time) * 1e-3f;
    last_time = t;

    gElapsedTime += dt * 0.5;

    if (gKeys[GLUT_KEY_LEFT]) gPosition.move_left(dt);
    if (gKeys[GLUT_KEY_RIGHT]) gPosition.move_right(dt);
    if (gKeys[GLUT_KEY_UP]) gPosition.move_forward(dt);
    if (gKeys[GLUT_KEY_DOWN]) gPosition.move_back(dt);
    if (gKeys[GLUT_KEY_PAGE_UP]) gPosition.move_up(dt);
    if (gKeys[GLUT_KEY_PAGE_DOWN]) gPosition.move_down(dt);

    gLightPosition = glm::vec3(gPosition.position.x + 1000.0, 100.0, gPosition.position.z - 1000.0);

    glutPostRedisplay();
}

void MouseMotion(int x, int y) {
    static bool wrap = false;

    if (!wrap) {
        gPosition.set_mouse_point(x, y);

        wrap = true;
        glutWarpPointer(gWindowWidth / 2, gWindowHeight / 2);
    } else {
        wrap = false;
    }
}

/*****************************************************************************
 * Main program
 ****************************************************************************/
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(Width, Height);
    glutCreateWindow(Title);
    glutSetCursor(GLUT_CURSOR_NONE);

    atexit(Deinit);
    if (!Init()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(SpecialKeys);
    glutSpecialUpFunc(SpecialKeysUp);
    glutIdleFunc(Idle);
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(MouseMotion);

    glutMainLoop();

    return 0;
}
