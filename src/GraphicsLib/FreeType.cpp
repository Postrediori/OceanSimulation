// FreeType.cpp
#include "stdafx.h"
#include "Shader.h"
#include "FreeType.h"

#define max(a,b) ((a)>(b)?(a):(b))

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

FontAtlas::FontAtlas(FT_Face face, FontSize_t height)
    : w(0)
    , h(0)
    , tex(0) {
    FT_Set_Pixel_Sizes(face, 0, height);
    FT_GlyphSlot g = face->glyph;

    memset(characters, 0, sizeof(characters));

    int roww = 0, rowh = 0;

    // Find minimum size for a texture holding all visible ASCII characters
    for (int i=FirstDisplayedCharacter; i<CharacterCount; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            LOGE << "Loading character " << i << " failed!";
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

    for (int i=FirstDisplayedCharacter; i<CharacterCount; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            LOGE << "Loading character " << i << " failed!";
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

        CharInfo inf;
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

    LOGD << "Generated a " << w << "x" << h << " (" << (w * h / 1024) << " kb) texture atlas";
}

FontAtlas::~FontAtlas() {
    if (tex) {
        glDeleteTextures(1, &tex);
    }
}

/*****************************************************************************
 * FontRenderer
 ****************************************************************************/
FontRenderer::FontRenderer()
    : glProgram(0)
    , glShaderV(0)
    , glShaderF(0)
    , aCoord(0)
    , uTex(0)
    , uColor(0)
    , vbo(0) {
}

bool FontRenderer::init(const std::string& vertex_shader, const std::string& fragment_shader) {
    if (!initObjects()) {
        return false;
    }

    // Init shader
    if (!Shader::createProgram(glProgram, glShaderV, glShaderF,
                               vertex_shader, fragment_shader)) {
        return false;
    }

    if (!initShaderVariables()) {
        return false;
    }

    return true;
}

bool FontRenderer::init() {
    if (!initObjects()) {
        return false;
    }

    if (!initShaderProgram()) {
        return false;
    }

    if (!initShaderVariables()) {
        return false;
    }

    return true;
}


bool FontRenderer::initObjects() {
    // Init FreeType
    if (FT_Init_FreeType(&ft)) {
        return false;
    }

    // Init VBO
    glGenBuffers(1, &vbo);
    if (!vbo) {
        return false;
    }

    return true;
}

bool FontRenderer::initShaderProgram() {
    if (Shader::createProgramSource(glProgram, glShaderV, glShaderF,
                                     vertex_src_1_30, fragment_src_1_30)) {
        LOGI << "Using GLSL 1.30 for Font Rendering";
        return true;
    }

    // ok, try and use OpenGL 2.1 then
    if (Shader::createProgramSource(glProgram, glShaderV, glShaderF,
                                     vertex_src_1_10, fragment_src_1_10)) {
        LOGI << "Using GLSL 1.10 for Font Rendering";
        return true;
    }

    return false;
}

bool FontRenderer::initShaderVariables() {
    aCoord = glGetAttribLocation(glProgram, "coord");
    uTex   = glGetUniformLocation(glProgram, "tex");
    uColor = glGetUniformLocation(glProgram, "color");

    if (aCoord==-1 || uTex==-1 || uColor==-1) {
        return false;
    }
    return true;
}

bool FontRenderer::load(const std::string& filename) {
    if (FT_New_Face(ft, filename.c_str(), 0, &face)) {
        return false;
    }
    return true;
}

FontHandle_t FontRenderer::createAtlas(FontSize_t height) {
    // Use font size as font handle.
    // TODO: Use unique number instead.
    FontHandle_t handle = static_cast<FontHandle_t>(height);
    fonts.emplace(std::make_pair(handle, FontAtlasGuard_t(new FontAtlas(face, height))));
    return handle;
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

void FontRenderer::renderText(FontHandle_t typeset,
                              FontArea area,
                              const std::string& text) {
    if (text.empty()) {
        return;
    }
    if (fonts.find(typeset) == fonts.end()) {
        return;
    }

    const auto& a = fonts[typeset];

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uTex, 0);
    glBindTexture(GL_TEXTURE_2D, a->tex);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(aCoord);
    glVertexAttribPointer(aCoord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    size_t len = text.length();
    std::vector<Coord2d> coords(6 * len);

    GLfloat tdx, tdy;
    float w, h;
    float x2, y2;
    float x = area.textx, y = area.texty;

    // Loop through all characters
    for (const char c : text) {
        CharInfo inf = a->characters[c];

        // Calculate vertex and texture coordinates
        x2 = x + inf.bl * area.sx;
        y2 = -y - inf.bt * area.sy;
        w = inf.bw * area.sx;
        h = inf.bh * area.sy;

        // Advance the cursor to the start of the next character
        x += inf.ax * area.sx;
        y += inf.ay * area.sy;

        // Skip glyphs that have no pixels
        if (!w || !h) {
            continue;
        }

        tdx = inf.bw/(float)a->w;
        tdy = inf.bh/(float)a->h;

        coords.push_back({x2+w, -y2, inf.tx+tdx, inf.ty});
        coords.push_back({x2, -y2-h, inf.tx, inf.ty+tdy});
        coords.push_back({x2+w, -y2-h, inf.tx+tdx, inf.ty+tdy});

        coords.push_back({x2, -y2, inf.tx, inf.ty});
        coords.push_back({x2, -y2-h, inf.tx, inf.ty+tdy});
        coords.push_back({x2+w, -y2, inf.tx+tdx, inf.ty});
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(Coord2d)*coords.size(), coords.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, coords.size());
}
