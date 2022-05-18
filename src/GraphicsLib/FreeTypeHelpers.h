// FreeTypeHelpers.h
#ifndef FTYPE_H
#define FTYPE_H

/*****************************************************************************
 * FontAtlas
 ****************************************************************************/
const int FirstDisplayedCharacter = 32;
const int CharacterCount = 128;
const int MaxWidth = 1024;

typedef int FontSize_t;

struct CharInfo {
    float ax, ay; // advance x and y
    float bw, bh; // bitmap width and height
    float bl, bt; // bitmap left and top
    float tx, ty; // x and y offsets in texture coords
};

class FontAtlas {
public:
    FontAtlas(FT_Face face, FontSize_t height);
    ~FontAtlas();

public:
    int w, h;
    GLuint tex;
    CharInfo characters[CharacterCount];
};

/*****************************************************************************
 * FontRenderer
 ****************************************************************************/
struct Coord2d {
    GLfloat x, y;
    GLfloat s, t;
	Coord2d(GLfloat x_, GLfloat y_, GLfloat s_, GLfloat t_)
		: x(x_), y(y_), s(s_), t(t_) { }
};

struct FontArea {
    float textx, texty;
    float sx, sy;
	FontArea(float tx, float ty, float sx_, float sy_)
		: textx(tx), texty(ty), sx(sx_), sy(sy_) { }
};

typedef unsigned int FontHandle_t;

typedef std::unique_ptr<FontAtlas> FontAtlasGuard_t;
typedef std::map<FontHandle_t, FontAtlasGuard_t> Fonts_t;

class FontRenderer {
public:
    FontRenderer();

    bool init();
    bool init(const std::string& vertex_shader, const std::string& fragment_shader);
    bool load(const std::string& filename);
    FontHandle_t createAtlas(FontSize_t height);

    void release();

    void renderStart();
    void renderEnd();
    void renderColor(const GLfloat *c);
    void renderText(FontHandle_t typeset,
                    FontArea area,
                    const std::string& text);

private:
    bool initObjects();
    bool initShaderProgram();
    bool initShaderVariables();

public:
    GLuint glProgram, glShaderV, glShaderF;
    GLint aCoord;
    GLint uTex, uColor;

    GLuint vbo;

    FT_Library ft;
    FT_Face face;

    Fonts_t fonts;
};

#endif
/* FTYPE_H */
