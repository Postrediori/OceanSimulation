// Ocean.cpp
#include "stdafx.h"
#include "Shader.h"
#include "Vector.h"
#include "Complex.h"
#include "FFT.h"
#include "GraphicsLogger.h"
#include "GraphicsResource.h"
#include "Ocean.h"

constexpr float Epsilon = 1e-6f;
#ifdef USE_OPENGL2_0
const std::filesystem::path VertexShader = "ocean110.vert";
const std::filesystem::path FragmentShader = "ocean110.frag";
#else
const std::filesystem::path VertexShader = "ocean.vert";
const std::filesystem::path FragmentShader = "ocean.frag";
#endif

constexpr float Lambda = -1.0;

float uniformRandomVariable() {
    return (float)rand() / RAND_MAX;
}

Complex gaussianRandomVariable() {
    float x1, x2, w;
    do {
        x1 = 2.0 * uniformRandomVariable() - 1.0;
        x2 = 2.0 * uniformRandomVariable() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while (w>=1.0);
    w = sqrt((-2.0 * log(w)) / w);
    return Complex(x1 * w, x2 * w);;
}

int Ocean::init(const std::filesystem::path& dataDir,
        const int N_, const float A_, const Vector2& w_, const float length_, int ocean_repeat_) {
    N = N_;
    Nplus1 = N + 1;
    A = A_;
    w = w_;
    length = length_;
    ocean_repeat = ocean_repeat_;

    h_tilde.resize(N * N);
    h_tilde_slopex.resize(N * N);
    h_tilde_slopez.resize(N * N);
    h_tilde_dx.resize(N * N);
    h_tilde_dz.resize(N * N);
    fft.init(N);

    // Use pre-calculated random values common for all model parameters
    // so that change of a parameter will be seamless.
    r.resize(Nplus1 * Nplus1 * 2);
    for (int i = 0; i < r.size(); i++) {
        r[i] = gaussianRandomVariable();
    }

    vertices.resize(Nplus1 * Nplus1);

    // lines
    indices_ln_count = 0;
    std::vector<GLuint> indices_ln(Nplus1 * Nplus1 * 10);

    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            int i = m_prime * Nplus1 + n_prime;

            indices_ln[indices_ln_count++] = i;
            indices_ln[indices_ln_count++] = i + 1;
            indices_ln[indices_ln_count++] = i;
            indices_ln[indices_ln_count++] = i + Nplus1;
            indices_ln[indices_ln_count++] = i;
            indices_ln[indices_ln_count++] = i + Nplus1 + 1;
            if (n_prime == N - 1) {
                indices_ln[indices_ln_count++] = i + 1;
                indices_ln[indices_ln_count++] = i + Nplus1 + 1;
            }
            if (m_prime == N - 1) {
                indices_ln[indices_ln_count++] = i + Nplus1;
                indices_ln[indices_ln_count++] = i + Nplus1 + 1;
            }
        }
    }

    // triangles
    indices_tr_count = 0;
    std::vector<GLuint> indices_tr(Nplus1 * Nplus1 * 10);

    for (int m_prime = 0; m_prime < N; m_prime++) {
        for (int n_prime = 0; n_prime < N; n_prime++) {
            int i = m_prime * Nplus1 + n_prime;

            indices_tr[indices_tr_count++] = i;
            indices_tr[indices_tr_count++] = i + Nplus1;
            indices_tr[indices_tr_count++] = i + Nplus1 + 1;
            indices_tr[indices_tr_count++] = i;
            indices_tr[indices_tr_count++] = i + Nplus1 + 1;
            indices_tr[indices_tr_count++] = i + 1;
        }
    }

    // create VBOs
    glGenBuffers(1, vertices_vbo.put()); LOGOPENGLERROR();
    if (!vertices_vbo) {
        return 0;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo.get()); LOGOPENGLERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(ocean_vertex) * Nplus1 * Nplus1,
        vertices.data(), GL_DYNAMIC_DRAW); LOGOPENGLERROR();

    glGenBuffers(1, indices_ln_vbo.put()); LOGOPENGLERROR();
    if (!indices_ln_vbo) {
        return 0;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_ln_vbo.get()); LOGOPENGLERROR();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_ln_count * sizeof(GLuint),
        indices_ln.data(), GL_STATIC_DRAW); LOGOPENGLERROR();

    glGenBuffers(1, indices_tr_vbo.put()); LOGOPENGLERROR();
    if (!indices_tr_vbo) {
        return 0;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_tr_vbo.get()); LOGOPENGLERROR();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_tr_count * sizeof(GLuint),
        indices_tr.data(), GL_STATIC_DRAW); LOGOPENGLERROR();

    // Init shader program
    auto vertexShader = dataDir / VertexShader;
    auto fragmentShader = dataDir / FragmentShader;
    glProgram.reset(Shader::CreateProgramFromFiles(vertexShader.string(), fragmentShader.string()));
    if (!glProgram) {
        LOGE << "Failed to init ocean shader program";
        return 0;
    }

    uLightPos = glGetUniformLocation(glProgram.get(), "light_pos"); LOGOPENGLERROR();
    uProjection = glGetUniformLocation(glProgram.get(), "projection"); LOGOPENGLERROR();
    uView = glGetUniformLocation(glProgram.get(), "view"); LOGOPENGLERROR();
    uModel = glGetUniformLocation(glProgram.get(), "model"); LOGOPENGLERROR();
#ifdef USE_OPENGL2_0
    uMVTranspInv = glGetUniformLocation(glProgram.get(), "mv_transp_inv"); LOGOPENGLERROR();
#endif

    uFogColor = glGetUniformLocation(glProgram.get(), "fog_color"); LOGOPENGLERROR();
    uEmissiveColor = glGetUniformLocation(glProgram.get(), "emissive_color"); LOGOPENGLERROR();
    uAmbientColor = glGetUniformLocation(glProgram.get(), "ambient_color"); LOGOPENGLERROR();
    uDiffuseColor = glGetUniformLocation(glProgram.get(), "diffuse_color"); LOGOPENGLERROR();
    uSpecularColor = glGetUniformLocation(glProgram.get(), "specular_color"); LOGOPENGLERROR();

    // Init vertex arrays
#ifndef USE_OPENGL2_0
    glGenVertexArrays(1, vao.put()); LOGOPENGLERROR();
    if (!vao) {
        return 0;
    }
    glBindVertexArray(vao.get()); LOGOPENGLERROR();
#endif

    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo.get()); LOGOPENGLERROR();

    aVertex = glGetAttribLocation(glProgram.get(), "vertex"); LOGOPENGLERROR();
    aNormal = glGetAttribLocation(glProgram.get(), "normal"); LOGOPENGLERROR();

    initBufferAttributes();

#ifndef USE_OPENGL2_0
    glBindVertexArray(0); LOGOPENGLERROR();
#endif

    initVertices();

    return 1;
}

void Ocean::initVertices() {
    for (int m_prime = 0; m_prime < Nplus1; m_prime++) {
        for (int n_prime = 0; n_prime < Nplus1; n_prime++) {
            int i = m_prime * Nplus1 + n_prime;

            Complex htilde0 = hTilde_0(n_prime, m_prime);
            Complex htilde0mk_conj = hTilde_0(-n_prime, -m_prime).conj();

            vertices[i].a = htilde0.a;
            vertices[i].b = htilde0.b;
            vertices[i]._a = htilde0mk_conj.a;
            vertices[i]._b = htilde0mk_conj.b;

            vertices[i].ox = vertices[i].x = (n_prime - N / 2.0) * length / N;
            vertices[i].oy = vertices[i].y = 0.0;
            vertices[i].oz = vertices[i].z = (m_prime - N / 2.0) * length / N;

            vertices[i].nx = 0.0;
            vertices[i].ny = 1.0;
            vertices[i].nz = 0.0;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo.get()); LOGOPENGLERROR();
    glBufferData(GL_ARRAY_BUFFER, sizeof(ocean_vertex)*Nplus1*Nplus1,
        vertices.data(), GL_DYNAMIC_DRAW); LOGOPENGLERROR();
}

void Ocean::initBufferAttributes() {
    glEnableVertexAttribArray(aVertex); LOGOPENGLERROR();
    glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, sizeof(ocean_vertex), 0); LOGOPENGLERROR();

    glEnableVertexAttribArray(aNormal); LOGOPENGLERROR();
    glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(ocean_vertex),
        (void*)(sizeof(GLfloat) * 3)); LOGOPENGLERROR();
}

float Ocean::dispersion(int n_prime, int m_prime) {
    float w_0 = 2.0 * M_PI / 200.0;
    float kx = M_PI * (2 * n_prime - N) / length;
    float kz = M_PI * (2 * m_prime - N) / length;
    return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

float Ocean::phillips(int n_prime, int m_prime) {
    Vector2 k(M_PI * (2 * n_prime - N) / length,
              M_PI * (2 * m_prime - N) / length);
    float k_length = k.length();
    if (k_length<Epsilon) {
        return 0.0;
    }

    float k_length2 = k_length  * k_length;
    float k_length4 = k_length2 * k_length2;

    float k_dot_w  = k.unit() * w.unit();
    float k_dot_w2 = k_dot_w * k_dot_w;

    float w_length = w.length();
    float L        = w_length * w_length / g;
    float L2       = L * L;

    float damping  = 0.001;
    float ld2      = L2 * damping * damping;

    return A * exp(-1.0 / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * ld2);
}

Complex Ocean::hTilde_0(int n_prime, int m_prime) {
    // Use pre-calculated random values from an array
    size_t idx = abs(m_prime) * Nplus1 + abs(n_prime)
        + ((n_prime < 0 && m_prime < 0) ? Nplus1 * Nplus1 : 0);
    return r[idx] * sqrt(phillips(n_prime, m_prime) / 2.0);
}

Complex Ocean::hTilde(float t, int n_prime, int m_prime) {
    int i = m_prime * Nplus1 + n_prime;

    Complex htilde0(vertices[i].a, vertices[i].b);
    Complex htilde0mkconj(vertices[i]._a, vertices[i]._b);

    float omegat = dispersion(n_prime, m_prime) * t;

    float cost = cos(omegat);
    float sint = sin(omegat);

    Complex c0(cost,  sint);
    Complex c1(cost, -sint);

    return htilde0 * c0 + htilde0mkconj * c1;
}

complex_vector_norm Ocean::h_D_and_n(Vector2 x, float t) {
    Complex h(0.0, 0.0);
    Vector2 D(0.0, 0.0);
    Vector3 n(0.0, 0.0, 0.0);

    for (int m_prime=0; m_prime<N; m_prime++) {
        float kz = 2.0 * M_PI * (m_prime - N / 2.0) / length;
        for (int n_prime=0; n_prime<N; n_prime++) {
            float kx = 2.0 * M_PI * (n_prime - N / 2.0) / length;
            Vector2 k = Vector2(kx, kz);

            float k_length = k.length();
            float k_dot_x = k * x;

            Complex c = Complex(cos(k_dot_x), sin(k_dot_x));
            Complex htilde_c = hTilde(t, n_prime, m_prime) * c;

            h = h + htilde_c;

            n = n + Vector3(-kx * htilde_c.b, 0.0, -kz * htilde_c.b);

            if (k_length < Epsilon) {
                continue;
            }
            D = D + Vector2(kx / k_length * htilde_c.b, kz / k_length * htilde_c.b);
        }
    }

    n = (Vector3(0.0, 1.0, 0.0) - n).unit();

    return complex_vector_norm(h, D, n);
}

void Ocean::evaluateWaves(float t) {
    for (int m_prime=0; m_prime<N; m_prime++) {
        for (int n_prime=0; n_prime<N; n_prime++) {
            int i = m_prime * Nplus1 + n_prime;

            Vector2 x = Vector2(vertices[i].x, vertices[i].z);

            complex_vector_norm h_d_and_n = h_D_and_n(x, t);

            vertices[i].y = h_d_and_n.h.a;

            vertices[i].x = vertices[i].ox + Lambda * h_d_and_n.D.x;
            vertices[i].z = vertices[i].oz + Lambda * h_d_and_n.D.y;

            vertices[i].nx = h_d_and_n.n.x;
            vertices[i].ny = h_d_and_n.n.y;
            vertices[i].nz = h_d_and_n.n.z;

            if (n_prime==0 && m_prime==0) {
                vertices[i+N+Nplus1*N].y = h_d_and_n.h.a;

                vertices[i+N+Nplus1*N].x = vertices[i+N+Nplus1*N].ox + Lambda*h_d_and_n.D.x;
                vertices[i+N+Nplus1*N].z = vertices[i+N+Nplus1*N].oz + Lambda*h_d_and_n.D.y;

                vertices[i+N+Nplus1*N].nx = h_d_and_n.n.x;
                vertices[i+N+Nplus1*N].ny = h_d_and_n.n.y;
                vertices[i+N+Nplus1*N].nz = h_d_and_n.n.z;
            }

            if (n_prime==0) {
                vertices[i+N].y = h_d_and_n.h.a;

                vertices[i+N].x = vertices[i+N].ox + Lambda*h_d_and_n.D.x;
                vertices[i+N].z = vertices[i+N].oz + Lambda*h_d_and_n.D.y;

                vertices[i+N].nx = h_d_and_n.n.x;
                vertices[i+N].ny = h_d_and_n.n.y;
                vertices[i+N].nz = h_d_and_n.n.z;
            }

            if (m_prime==0) {
                vertices[i+Nplus1*N].y = h_d_and_n.h.a;

                vertices[i+Nplus1*N].x = vertices[i+Nplus1*N].ox + Lambda*h_d_and_n.D.x;
                vertices[i+Nplus1*N].z = vertices[i+Nplus1*N].oz + Lambda*h_d_and_n.D.y;

                vertices[i+Nplus1*N].nx = h_d_and_n.n.x;
                vertices[i+Nplus1*N].ny = h_d_and_n.n.y;
                vertices[i+Nplus1*N].nz = h_d_and_n.n.z;
            }
        }
    }
}

void Ocean::evaluateWavesFFT(float t) {
    for (int m_prime=0; m_prime<N; m_prime++) {
        float kz = M_PI * (2 * m_prime - N) / length;
        for (int n_prime=0; n_prime<N; n_prime++) {
            float kx = M_PI * (2 * n_prime - N) / length;
            float len = sqrt(kx * kx + kz * kz);
            int i = m_prime * N + n_prime;

            h_tilde[i] = hTilde(t, n_prime, m_prime);
            h_tilde_slopex[i] = h_tilde[i] * Complex(0.0, kx);
            h_tilde_slopez[i] = h_tilde[i] * Complex(0.0, kz);

            if (len<Epsilon) {
                h_tilde_dx[i] = Complex(0.0, 0.0);
                h_tilde_dz[i] = Complex(0.0, 0.0);
            } else {
                h_tilde_dx[i] = h_tilde[i] * Complex(0.0, -kx / len);
                h_tilde_dz[i] = h_tilde[i] * Complex(0.0, -kz / len);
            }
        }
    }

    for (int m_prime=0; m_prime<N; m_prime++) {
        fft.fft(h_tilde,        h_tilde,        1, m_prime * N);
        fft.fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
        fft.fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
        fft.fft(h_tilde_dx,     h_tilde_dx,     1, m_prime * N);
        fft.fft(h_tilde_dz,     h_tilde_dz,     1, m_prime * N);
    }
    for (int n_prime=0; n_prime<N; n_prime++) {
        fft.fft(h_tilde,        h_tilde,        N, n_prime);
        fft.fft(h_tilde_slopex, h_tilde_slopex, N, n_prime);
        fft.fft(h_tilde_slopez, h_tilde_slopez, N, n_prime);
        fft.fft(h_tilde_dx,     h_tilde_dx,     N, n_prime);
        fft.fft(h_tilde_dz,     h_tilde_dz,     N, n_prime);
    }

    const float signs[] = {1.0, -1.0};

    for (int m_prime=0; m_prime<N; m_prime++) {
        for (int n_prime=0; n_prime<N; n_prime++) {
            // index into h_tilde
            int i  = m_prime * N      + n_prime;

            // index onto vertices
            int i1 = m_prime * Nplus1 + n_prime;

            int sign = signs[(n_prime + m_prime) & 1];

            h_tilde[i] = h_tilde[i] * sign;

            // height
            vertices[i1].y = h_tilde[i].a;

            // displacement
            h_tilde_dx[i] = h_tilde_dx[i] * sign;
            h_tilde_dz[i] = h_tilde_dz[i] * sign;
            vertices[i1].x = vertices[i1].ox + h_tilde_dx[i].a * Lambda;
            vertices[i1].z = vertices[i1].oz + h_tilde_dz[i].a * Lambda;

            // normal
            h_tilde_slopex[i] = h_tilde_slopex[i] * sign;
            h_tilde_slopez[i] = h_tilde_slopez[i] * sign;

            Vector3 n = Vector3(0.0-h_tilde_slopex[i].a, 1.0, 0.0-h_tilde_slopez[i].a).unit();
            vertices[i1].nx = n.x;
            vertices[i1].ny = n.y;
            vertices[i1].nz = n.z;

            // tiling
            if (n_prime==0 && m_prime==0) {
                vertices[i1+N+Nplus1*N].y = h_tilde[i].a;

                vertices[i1+N+Nplus1*N].x = vertices[i1+N+Nplus1*N].ox + Lambda*h_tilde_dx[i].a;
                vertices[i1+N+Nplus1*N].z = vertices[i1+N+Nplus1*N].oz + Lambda*h_tilde_dz[i].a;

                vertices[i1+N+Nplus1*N].nx = n.x;
                vertices[i1+N+Nplus1*N].ny = n.y;
                vertices[i1+N+Nplus1*N].nz = n.z;
            }

            if (n_prime==0) {
                vertices[i1+N].y = h_tilde[i].a;

                vertices[i1+N].x = vertices[i1+N].ox + Lambda*h_tilde_dx[i].a;
                vertices[i1+N].z = vertices[i1+N].oz + Lambda*h_tilde_dz[i].a;

                vertices[i1+N].nx = n.x;
                vertices[i1+N].ny = n.y;
                vertices[i1+N].nz = n.z;
            }

            if (m_prime==0) {
                vertices[i1+Nplus1*N].y = h_tilde[i].a;

                vertices[i1+Nplus1*N].x = vertices[i1+Nplus1*N].ox + Lambda*h_tilde_dx[i].a;
                vertices[i1+Nplus1*N].z = vertices[i1+Nplus1*N].oz + Lambda*h_tilde_dz[i].a;

                vertices[i1+Nplus1*N].nx = n.x;
                vertices[i1+Nplus1*N].ny = n.y;
                vertices[i1+Nplus1*N].nz = n.z;
            }
        }
    }
}

void Ocean::render(float t, const glm::vec3& light_pos, const glm::mat4& proj,
                   const glm::mat4& view, const glm::mat4& model, bool use_fft) {
    static bool eval = false;
    if (!use_fft && !eval) {
        eval = true;
        evaluateWaves(t);
    } else if (use_fft) {
        evaluateWavesFFT(t);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo.get()); LOGOPENGLERROR();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ocean_vertex)*Nplus1*Nplus1, vertices.data()); LOGOPENGLERROR();

    glUseProgram(glProgram.get()); LOGOPENGLERROR();

    glUniform3f(uLightPos, light_pos.x, light_pos.y, light_pos.z); LOGOPENGLERROR();
    glUniformMatrix4fv(uProjection,  1, GL_FALSE, glm::value_ptr(proj)); LOGOPENGLERROR();
    glUniformMatrix4fv(uView,        1, GL_FALSE, glm::value_ptr(view)); LOGOPENGLERROR();

    glUniform4fv(uFogColor, 1, fogColor); LOGOPENGLERROR();
    glUniform4fv(uEmissiveColor, 1, emissiveColor); LOGOPENGLERROR();
    glUniform4fv(uAmbientColor, 1, ambientColor); LOGOPENGLERROR();
    glUniform4fv(uDiffuseColor, 1, diffuseColor); LOGOPENGLERROR();
    glUniform4fv(uSpecularColor, 1, specularColor); LOGOPENGLERROR();

#ifndef USE_OPENGL2_0
    glBindVertexArray(vao.get()); LOGOPENGLERROR();
#else
    initBufferAttributes();
#endif

    GLenum geometry = 0;
    GLuint indices_vbo = 0;
    GLsizei indices_count = 0;
    switch (geometry_type) {
    case GeometryRenderType::Solid:
        geometry = GL_TRIANGLES;
        indices_vbo = indices_tr_vbo.get();
        indices_count = indices_tr_count;
        break;
    case GeometryRenderType::Wireframe:
        geometry = GL_LINES;
        indices_vbo = indices_ln_vbo.get();
        indices_count = indices_ln_count;
        break;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo); LOGOPENGLERROR();

    static const float ocean_scale = 1.f;
    for (int i=0; i<ocean_repeat; i++) {
        for (int j=0; j<ocean_repeat; j++) {
            glm::mat4 m = model;
            m = glm::scale(m, glm::vec3(ocean_scale));
            m = glm::translate(m, glm::vec3(length * (-ocean_repeat/2 + i + 0.5), 0.0,
                                            length * ( ocean_repeat/2 - j - 0.5)));

            glUniformMatrix4fv(uModel,       1, GL_FALSE, glm::value_ptr(m)); LOGOPENGLERROR();

#ifdef USE_OPENGL2_0
            // Pass inverse(transpose(view * model)) as uniform as GLSL 1.10 doesn't have these functions
            glm::mat4 mv_transp_inv = glm::inverse(glm::transpose(view * m));
            glUniformMatrix4fv(uMVTranspInv, 1, GL_FALSE, glm::value_ptr(mv_transp_inv));
#endif

            glDrawElements(geometry, indices_count, GL_UNSIGNED_INT, 0); LOGOPENGLERROR();
        }
    }

#ifndef USE_OPENGL2_0
    glBindVertexArray(0); LOGOPENGLERROR();
#endif
    glUseProgram(0); LOGOPENGLERROR();
}

void Ocean::geometryType(GeometryRenderType t) {
    geometry_type = t;
}

void Ocean::colors(float fog[], float emissive[], float ambient[], float diffuse[], float specular[]) {
    for (size_t i = 0; i < 4; i++) {
        fogColor[i] = fog[i];
        emissiveColor[i] = emissive[i];
        ambientColor[i] = ambient[i];
        diffuseColor[i] = diffuse[i];
        specularColor[i] = specular[i];
    }
}

void Ocean::windAmp(float newA) {
    A = newA;
    initVertices();
}

void Ocean::windDirZ(float newWindZ) {
    w = Vector2(w.x, newWindZ);
    initVertices();
}
