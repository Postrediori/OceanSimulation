// Ocean.h
#pragma once

class Complex;
class Vector2;
class Vector3;
class FTT;

struct ocean_vertex {
    GLfloat  x,  y,  z; // vertex
    GLfloat nx, ny, nz; // normal
    GLfloat  a,  b,  c; // htilde0
    GLfloat _a, _b, _c; // htilde0mk conjugate
    GLfloat ox, oy, oz; // original position
};

// structure used with discrete Fourier transform
struct complex_vector_norm {
    Complex h; // wave height
    Vector2 D; // displacement
    Vector3 n; // normal

    complex_vector_norm() { }
    complex_vector_norm(Complex ch, Vector2 cD, Vector3 cn)
        : h(ch), D(cD), n(cn) { }
};

enum GEOMETRY_TYPE : int {
    GEOMETRY_LINES = 0,
    GEOMETRY_SOLID = 1,

    GEOMETRY_TYPES
};

/*****************************************************************************
 * Ocean
 ****************************************************************************/
class Ocean {
public:
    Ocean();
    ~Ocean();

    int init(const int N, const float A, const Vector2& w, const float length, const int ocean_repeat);
    void release();

    void render(float t, const glm::vec3& light_pos,
                const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model,
                bool use_fft);

    void geometryType(GEOMETRY_TYPE t);

    void colors(float fog[], float emissive[], float ambient[], float diffuse[], float specular[]);

    void windAmp(float newA);
    void windDirZ(float newWindZ);

private:
    void initVertices();

    // deep water
    float dispersion(int n_prime, int m_prime);

    // Phillips spectrum
    float phillips(int n_prime, int m_prime);

    Complex hTilde_0(int n_prime, int m_prime);
    Complex hTilde(float t, int n_prime, int m_prime);
    complex_vector_norm h_D_and_n(Vector2 x, float t);
    void evaluateWaves(float t);
    void evaluateWavesFFT(float t);

private:
    GEOMETRY_TYPE geometry_type = GEOMETRY_TYPE::GEOMETRY_SOLID;

    // gravity constant
    float g = 9.81f;

    // dimension - N should be a power of 2
    int N = 1, Nplus1 = 2;

    // Phillips spectrum parameter - affects heights of waves
    float A = 0.0f;

    // wind parameter
    Vector2 w;

    // length parameter
    float length = 1.0f;

    // number of repeating ocean surface along each axis
    int ocean_repeat = 1;

    // fast Fourier transform parameters
    std::vector<Complex> h_tilde;
    std::vector<Complex> h_tilde_slopex;
    std::vector<Complex> h_tilde_slopez;
    std::vector<Complex> h_tilde_dx;
    std::vector<Complex> h_tilde_dz;

    std::vector<Complex> r;

    // fast Fourier transform
    FFT fft;

    // vertices for VBO
    std::vector<ocean_vertex> vertices;

    // indices for VBO
    // number of indices to render
    unsigned int indices_ln_count = 0;
    unsigned int indices_tr_count = 0;

    // VAOs
    GLuint vao = 0;
    
    // VBOs
    GLuint vertices_vbo = 0;
    GLuint indices_ln_vbo = 0;
    GLuint indices_tr_vbo = 0;

    // shaders
    GLuint glProgram = 0;

    // attributes and uniforms
    GLint uLightPos = -1, uProjection = -1, uView = -1, uModel = -1;

    GLint uFogColor = -1, uEmissiveColor = -1, uAmbientColor = -1,
        uDiffuseColor = -1, uSpecularColor = -1;

    float fogColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float emissiveColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float ambientColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float diffuseColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float specularColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};
