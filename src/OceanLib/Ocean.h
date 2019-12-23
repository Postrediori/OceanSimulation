// Ocean.h
#ifndef OCEAN_H
#define OCEAN_H

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
    Complex   h; // wave height
    Vector2 D; // displacement
    Vector3 n; // normal
};

enum GEOMETRY_TYPE {
    GEOMETRY_LINES,
    GEOMETRY_SOLID,

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

    // deep water
    float dispersion(int n_prime, int m_prime);

    // Phillips spectrum
    float phillips(int n_prime, int m_prime);

    Complex hTilde_0(int n_prime, int m_prime);
    Complex hTilde(float t, int n_prime, int m_prime);
    complex_vector_norm h_D_and_n(const Vector2& x, float t);
    void evaluateWaves(float t);
    void evaluateWavesFFT(float t);

    void render(float t, const glm::vec3& light_pos,
                const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model,
                bool use_fft);

    void geometryType(GEOMETRY_TYPE t);

private:
    int initShaderProgram();
    void initAttributes();

private:
    GEOMETRY_TYPE geometry_type;

    // gravity constant
    float g;

    // dimension - N should be a power of 2
    int N, Nplus1;

    // Phillips spectrum parameter - affects heights of waves
    float A;

    // wind parameter
    Vector2 w;

    // length parameter
    float length;

    // number of repeating ocean surface along each axis
    int ocean_repeat;

    // fast Fourier transform parameters
    Complex *h_tilde,
            *h_tilde_slopex, *h_tilde_slopez,
            *h_tilde_dx, *h_tilde_dz;

    // fast Fourier transform
    FFT *fft;

    // vertices for VBO
    ocean_vertex *vertices;

    // indices for VBO
    unsigned int *indices_ln, *indices_tr;

    // number of indices to render
    unsigned int indices_ln_count, indices_tr_count;

    // version of shader ar integer (i.e. 110 for 1.10)
    int shaderVersion;

    // VAOs
    GLuint vao;
    
    // VBOs
    GLuint vertices_vbo, indices_ln_vbo, indices_tr_vbo;

    // shaders
    GLuint glProgram, glShaderV, glShaderF;

    // attributes and uniforms
    GLint aVertex, aNormal, aTexture;
    GLint uLightPos, uProjection, uView, uModel, uMVTranspInv;
};

#endif /* OCEAN_H */
