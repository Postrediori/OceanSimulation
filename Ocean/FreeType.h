// FreeType.h
#ifndef FTYPE_H
#define FTYPE_H

/*****************************************************************************
 * FontAtlas
 ****************************************************************************/

const int CharacterCount = 128;
const int MaxWidth = 1024;

typedef struct _CHARINFO_ {
    float ax, ay; // advance x and y
    float bw, bh; // bitmap width and height
    float bl, bt; // bitmap left and top
    float tx, ty; // x and y offsets in texture coords
} CHARINFO;

struct FontAtlas {
    GLuint tex;
    int w, h;
    CHARINFO characters[CharacterCount];

    FontAtlas(FT_Face face, const int height);
    ~FontAtlas();
};

/*****************************************************************************
 * FontRenderer
 ****************************************************************************/
typedef struct _COORD2D_ {
    GLfloat x, y;
    GLfloat s, t;
} COORD2D;

struct FontRenderer {
    GLuint glProgram, glShaderV, glShaderF;
    GLint aCoord;
    GLint uTex, uColor;

    GLuint vbo;

    FT_Library ft;
    FT_Face face;

    int init();
    int init(const char * vertex_shader, const char * fragment_shader);
    int load(const char * filename);
    FontAtlas* createAtlas(const int height);

    void release();

    void renderStart();
    void renderEnd();
    void renderColor(const GLfloat *c);
    void renderText(const FontAtlas *a,
                    const float textx, const float texty,
                    const float sx, const float sy,
                    const char *fmt, ...);
};

#endif
/* FTYPE_H */
