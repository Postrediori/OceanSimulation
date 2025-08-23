// Ocean.h
#pragma once

class Complex;
class Vector2;
class Vector3;
class FTT;

#pragma pack(push, 0)
struct ocean_vertex {
    GLfloat  x{ 0.0 },  y{ 0.0 },  z{ 0.0 }; // vertex
    GLfloat nx{ 0.0 }, ny{ 0.0 }, nz{ 0.0 }; // normal
    GLfloat  a{ 0.0 },  b{ 0.0 },  c{ 0.0 }; // htilde0
    GLfloat _a{ 0.0 }, _b{ 0.0 }, _c{ 0.0 }; // htilde0mk conjugate
    GLfloat ox{ 0.0 }, oy{ 0.0 }, oz{ 0.0 }; // original position
};
#pragma pack(pop)

// structure used with discrete Fourier transform
struct complex_vector_norm {
    Complex h; // wave height
    Vector2 D; // displacement
    Vector3 n; // normal

    complex_vector_norm() = default;
    complex_vector_norm(const Complex& ch, const Vector2& cD, const Vector3& cn)
        : h(ch), D(cD), n(cn) { }
};

enum class GeometryRenderType : int {
    Wireframe,
    Solid
};

/*****************************************************************************
 * Ocean
 ****************************************************************************/
class Ocean {
public:
    Ocean() = default;

    bool init(const std::filesystem::path& dataDir,
        const int N, const float A, const Vector2& w, const float length, const int ocean_repeat);

    void render(const glm::vec3& light_pos,
                const glm::mat4& proj, const glm::mat4& view, const glm::mat4& model);

    void evaluate(float t, bool use_fft);

    void geometryType(GeometryRenderType t);

    void colorFog(const ColorInfo& fog);
    void colorEmissive(const ColorInfo& emissive);
    void colorAmbient(const ColorInfo& ambient);
    void colorDiffuse(const ColorInfo& diffuse);
    void colorSpecular(const ColorInfo& specular);

    void windAmp(float newA);
    void windDirZ(float newWindZ);

private:
    void initVertices();
    void initBufferAttributes();

    // deep water
    float dispersion(int n_prime, int m_prime);

    // Phillips spectrum
    float phillips(int n_prime, int m_prime);

    Complex hTilde_0(int n_prime, int m_prime);
    Complex hTilde(float t, int n_prime, int m_prime);
    complex_vector_norm h_D_and_n(const Vector2& x, float t);
    void evaluateWaves(float t);
    void evaluateWavesFFT(float t);

private:
    GeometryRenderType geometry_type = GeometryRenderType::Solid;

    // gravity constant
    float g = 9.81f;

    // dimension - N should be a power of 2
    int N = 1, Nplus1 = 2;

    // Phillips spectrum parameter - affects heights of waves
    float A = 0.0f;

    // wind parameter
    Vector2 wind;

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
    bool fftEval = false;

    // vertices for VBO
    std::vector<ocean_vertex> vertices;

    // indices for VBO
    // number of indices to render
    GLsizei indices_ln_count = 0;
    GLsizei indices_tr_count = 0;

#ifndef USE_OPENGL2_0
    // VAOs
    GraphicsUtils::unique_vertex_array vao;
#endif

    // VBOs
    GraphicsUtils::unique_buffer vertices_vbo;
    GraphicsUtils::unique_buffer indices_ln_vbo;
    GraphicsUtils::unique_buffer indices_tr_vbo;

    // shaders
    GraphicsUtils::unique_program glProgram;

    // attributes and uniforms
    GLint aVertex = -1, aNormal = -1;
    GLint uLightPos = -1, uProjection = -1, uView = -1, uModel = -1;
#ifdef USE_OPENGL2_0
    // Additional uniform for passing inverse(transpose(view * model));
    GLint uMVTranspInv = -1;
#endif

    GLint uFogColor = -1, uEmissiveColor = -1, uAmbientColor = -1,
        uDiffuseColor = -1, uSpecularColor = -1;

    ColorInfo fogColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    ColorInfo emissiveColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    ColorInfo ambientColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    ColorInfo diffuseColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    ColorInfo specularColor{ 0.0f, 0.0f, 0.0f, 1.0f };
};
