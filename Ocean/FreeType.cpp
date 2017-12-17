// FreeType.cpp
#include "stdafx.h"
#include "Shader.h"
#include "FreeType.h"

#define max(a,b) ((a)>(b)?(a):(b))

FontAtlas::FontAtlas(FT_Face face, const int height) {
    FT_Set_Pixel_Sizes(face, 0, height);
    FT_GlyphSlot g = face->glyph;

    memset(characters, 0, sizeof(characters));

    w = 0;
    h = 0;
    int roww = 0, rowh = 0;

    // Find minimum size for a texture holding all visible ASCII characters
    for (int i=32; i<CharacterCount; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            std::cerr<<"Loading character "<<i<<" failed!"<<std::endl;
            continue;
        }

        if (roww+g->bitmap.width+1>=MaxWidth) {
            w = max(w, roww);
            h += rowh;
            roww = 0;
            rowh = 0;
        }

        roww += g->bitmap.width + 1;
        rowh = max(rowh, g->bitmap.rows);
    }

    w = max(w, roww);
    h += rowh;

    // Create texture for all ASCII glyphs
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    // 1-byte alignment required when uploading texture data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Clamping edges is important to prevent artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Linear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Paste all glyph bitmaps into the texture, remembering offset
    int ox = 0, oy = 0;

    rowh = 0;

    for (int i=32; i<CharacterCount; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            std::cerr<<"Loading character "<<i<<" failed!"<<std::endl;
            continue;
        }

        if ((ox+g->bitmap.width+1)>=MaxWidth) {
            oy += rowh;
            rowh = 0;
            ox = 0;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy,
                        g->bitmap.width, g->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        CHARINFO inf;
        inf.ax = g->advance.x >> 6;
        inf.ay = g->advance.y >> 6;

        inf.bw = g->bitmap.width;
        inf.bh = g->bitmap.rows;

        inf.bl = g->bitmap_left;
        inf.bt = g->bitmap_top;

        inf.tx = ox / (float)w;
        inf.ty = oy / (float)h;

        characters[i] = inf;

        rowh = max(rowh, g->bitmap.rows);
        ox += g->bitmap.width + 1;
    }

    std::cout<<"Generated a "<<w<<"x"<<h<<" ("<<(w*h/1024)<<" kb) texture atlas"<<std::endl;
}

FontAtlas::~FontAtlas() {
    glDeleteTextures(1, &tex);
}

int FontRenderer::init(const char * vertex_shader, const char * fragment_shader) {
    // Init FreeType
    if (FT_Init_FreeType(&ft)) return 0;

    // Init VBO
    glGenBuffers(1, &vbo);

    // Init shader
    Shader::createProgram(glProgram, glShaderV, glShaderF,
                          vertex_shader, fragment_shader);
    if (!glProgram) return 0;

    aCoord = glGetAttribLocation(glProgram, "coord");
    uTex   = glGetUniformLocation(glProgram, "tex");
    uColor = glGetUniformLocation(glProgram, "color");

    if (aCoord==-1 || uTex==-1 || uColor==-1) return 0;

    return 1;
}

int FontRenderer::init() {
    // Init FreeType
    if (FT_Init_FreeType(&ft)) return 0;

    // Init VBO
    glGenBuffers(1, &vbo);

    // Init shader
    const char * vertex_src;
    const char * fragment_src;
    if (GLEW_VERSION_3_0) {
        static const char vertex_src_1_30[] =
            "#version 130\n"
            "in vec4 coord;"
            "out vec2 tex_coord;"
            "void main(){"
            "    gl_Position=vec4(coord.xy,0.,1.);"
            "    tex_coord=coord.zw;"
            "}";
        static const char fragment_src_1_30[] =
            "#version 130\n"
            "in vec2 tex_coord;"
            "out vec4 frag_color;"
            "uniform vec4 color;"
            "uniform sampler2D tex;"
            "void main(){"
            "    float a=texture(tex,tex_coord).r;"
            "    frag_color=vec4(1.,1.,1.,a)*color;"
            "}";
        vertex_src = vertex_src_1_30;
        fragment_src = fragment_src_1_30;
    } else {
        static const char vertex_src_1_10[] =
            "#version 110\n"
            "attribute vec4 coord;"
            "varying vec2 tex_coord;"
            "void main(){"
            "    gl_Position=vec4(coord.xy,0.,1.);"
            "    tex_coord=coord.zw;"
            "}";
        static const char fragment_src_1_10[] =
            "#version 110\n"
            "varying vec2 tex_coord;"
            "uniform vec4 color;"
            "uniform sampler2D tex;"
            "void main(){"
            "    float a=texture2D(tex,tex_coord).r;"
            "    gl_FragColor=vec4(1.,1.,1.,a)*color;"
            "}";
        vertex_src = vertex_src_1_10;
        fragment_src = fragment_src_1_10;
    }

    Shader::createProgramSource(glProgram, glShaderV, glShaderF,
                                vertex_src, fragment_src);
    if (!glProgram) return 0;

    aCoord = glGetAttribLocation(glProgram, "coord");
    uTex   = glGetUniformLocation(glProgram, "tex");
    uColor = glGetUniformLocation(glProgram, "color");

    if (aCoord==-1 || uTex==-1 || uColor==-1) return 0;

    return 1;
}

int FontRenderer::load(const char * filename) {
    if (FT_New_Face(ft, filename, 0, &face)) return 0;
    return 1;
}

FontAtlas * FontRenderer::createAtlas(const int height) {
    FontAtlas * a = new FontAtlas(face, height);
    return a;
}

void FontRenderer::release() {
    Shader::releaseProgram(glProgram, glShaderV, glShaderF);
    glDeleteBuffers(1, &vbo);
}

void FontRenderer::renderStart() {
    glPushAttrib(GL_COLOR_BUFFER_BIT); // Push GL_BLEND and Blending function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(glProgram);
}

void FontRenderer::renderEnd() {
    glUseProgram(0);
    glPopAttrib();
}

void FontRenderer::renderColor(const GLfloat c[]) {
    glUniform4fv(uColor, 1, c);
}

void FontRenderer::renderText(const FontAtlas * a,
                              const float textx, const float texty,
                              const float sx, const float sy,
                              const char * fmt, ...) {
    static char text[256];
    va_list ap;

    if (fmt==NULL) return;

    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uTex, 0);
    glBindTexture(GL_TEXTURE_2D, a->tex);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(aCoord);
    glVertexAttribPointer(aCoord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    int len = strlen(text);
    COORD2D * coords = new COORD2D[6*len];
    int c = 0;

    // Loop through all characters
    GLfloat tdx, tdy;
    float w, h;
    float x2, y2;
    float x = textx, y = texty;
    for (const unsigned char * p=(const unsigned char *)text; *p; p++) {
        CHARINFO inf = a->characters[*p];

        // Calculate vertex and texture coordinates
        x2 = x + inf.bl * sx;
        y2 = -y - inf.bt * sy;
        w = inf.bw * sx;
        h = inf.bh * sy;

        // Advance the cursor to the start of the next character
        x += inf.ax * sx;
        y += inf.ay * sy;

        // Skip glyphs that have no pixels
        if (!w || !h) continue;

        tdx = inf.bw/(float)a->w;
        tdy = inf.bh/(float)a->h;

        COORD2D crd;
        crd.x = x2+w;
        crd.y = -y2;
        crd.s = inf.tx+tdx;
        crd.t = inf.ty;
        coords[c++] = crd;

        crd.x = x2;
        crd.y = -y2-h;
        crd.s = inf.tx;
        crd.t = inf.ty+tdy;
        coords[c++] = crd;

        crd.x = x2+w;
        crd.y = -y2-h;
        crd.s = inf.tx+tdx;
        crd.t = inf.ty+tdy;
        coords[c++] = crd;

        crd.x = x2;
        crd.y = -y2;
        crd.s = inf.tx;
        crd.t = inf.ty;
        coords[c++] = crd;

        crd.x = x2;
        crd.y = -y2-h;
        crd.s = inf.tx;
        crd.t = inf.ty+tdy;
        coords[c++] = crd;

        crd.x = x2+w;
        crd.y = -y2;
        crd.s = inf.tx+tdx;
        crd.t = inf.ty;
        coords[c++] = crd;
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(COORD2D)*6*len, coords, GL_DYNAMIC_DRAW);
    delete [] coords;

    glDrawArrays(GL_TRIANGLES, 0, c);
}
