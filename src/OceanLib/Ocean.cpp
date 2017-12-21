// Ocean.cpp
#include "stdafx.h"
#include "Shader.h"
#include "Vector.h"
#include "Complex.h"
#include "FFT.h"
#include "Ocean.h"

static const float Epsilon = 1e-6f;
static const char vertex_src_1_10[] = "data/ocean110.vert";
static const char fragment_src_1_10[] = "data/ocean110.frag";
static const char vertex_src_1_30[] = "data/ocean130.vert";
static const char fragment_src_1_30[] = "data/ocean130.frag";

static float uniformRandomVariable() {
    return (float)rand() / RAND_MAX;
}

static Complex gaussianRandomVariable() {
    float x1, x2, w;
    do {
        x1 = 2.0 * uniformRandomVariable() - 1.0;
        x2 = 2.0 * uniformRandomVariable() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while (w>=1.0);
    w = sqrt((-2.0 * log(w)) / w);
    return Complex(x1 * w, x2 * w);;
}

Ocean::Ocean()
    : g(9.81f)
    , vertices(0)
    , indices_ln(0)
    , indices_tr(0)
    , shaderVersion(0)
    , vao(0)
    , h_tilde(0)
    , h_tilde_slopex(0)
    , h_tilde_slopez(0)
    , h_tilde_dx(0)
    , h_tilde_dz(0)
    , fft(0)
    , geometry_type(GEOMETRY_SOLID) {
    //
}

Ocean::~Ocean() {
    release();
}

int Ocean::init(const int N, const float A, const Vector2& w, const float length, int ocean_repeat) {
    this->N = N;
    Nplus1 = N+1;
    this->A = A;
    this->w = w;
    this->length = length;
    this->ocean_repeat = ocean_repeat;

    h_tilde        = new Complex[N*N];
    h_tilde_slopex = new Complex[N*N];
    h_tilde_slopez = new Complex[N*N];
    h_tilde_dx     = new Complex[N*N];
    h_tilde_dz     = new Complex[N*N];
    fft            = new FFT(N);
    vertices       = new ocean_vertex[Nplus1*Nplus1];
    indices_ln     = new unsigned int[Nplus1*Nplus1*10];
    indices_tr     = new unsigned int[Nplus1*Nplus1*10];

    int i;

    Complex htilde0, htilde0mk_conj;
    for (int m_prime=0; m_prime<Nplus1; m_prime++) {
        for (int n_prime=0; n_prime<Nplus1; n_prime++) {
            i = m_prime * Nplus1 + n_prime;

            htilde0        = hTilde_0( n_prime,  m_prime);
            htilde0mk_conj = hTilde_0(-n_prime, -m_prime).conj();

            vertices[i].a  = htilde0.a;
            vertices[i].b  = htilde0.b;
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

    // lines
    indices_ln_count = 0;
    for (int m_prime=0; m_prime<N; m_prime++) {
        for (int n_prime=0; n_prime<N; n_prime++) {
            i = m_prime * Nplus1 + n_prime;

            indices_ln[indices_ln_count++] = i;
            indices_ln[indices_ln_count++] = i + 1;
            indices_ln[indices_ln_count++] = i;
            indices_ln[indices_ln_count++] = i + Nplus1;
            indices_ln[indices_ln_count++] = i;
            indices_ln[indices_ln_count++] = i + Nplus1 + 1;
            if (n_prime==N-1) {
                indices_ln[indices_ln_count++] = i + 1;
                indices_ln[indices_ln_count++] = i + Nplus1 + 1;
            }
            if (m_prime==N-1) {
                indices_ln[indices_ln_count++] = i + Nplus1;
                indices_ln[indices_ln_count++] = i + Nplus1 + 1;
            }
        }
    }

    // triangles
    indices_tr_count = 0;
    for (int m_prime=0; m_prime<N; m_prime++) {
        for (int n_prime=0; n_prime<N; n_prime++) {
            i = m_prime * Nplus1 + n_prime;

            indices_tr[indices_tr_count++] = i;
            indices_tr[indices_tr_count++] = i + Nplus1;
            indices_tr[indices_tr_count++] = i + Nplus1 + 1;
            indices_tr[indices_tr_count++] = i;
            indices_tr[indices_tr_count++] = i + Nplus1 + 1;
            indices_tr[indices_tr_count++] = i + 1;
        }
    }

    // create VBOs
    glGenBuffers(1, &vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ocean_vertex)*Nplus1*Nplus1,
                 vertices, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &indices_ln_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_ln_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_ln_count*sizeof(GLuint),
                 indices_ln, GL_STATIC_DRAW);

    glGenBuffers(1, &indices_tr_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_tr_vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_tr_count*sizeof(GLuint),
                 indices_tr, GL_STATIC_DRAW);


    if (!initShaderProgram()) {
        return 0;
    }

    aVertex      = glGetAttribLocation(glProgram, "vertex");
    aNormal      = glGetAttribLocation(glProgram, "normal");
    aTexture     = glGetAttribLocation(glProgram, "texture");
    uLightPos    = glGetUniformLocation(glProgram, "light_pos");
    uProjection  = glGetUniformLocation(glProgram, "projection");
    uView        = glGetUniformLocation(glProgram, "view");
    uModel       = glGetUniformLocation(glProgram, "model");
    uMVTranspInv = glGetUniformLocation(glProgram, "mv_transp_inv");

    // Create VAOs
    if (shaderVersion >= 130) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        initAttributes();

        glBindVertexArray(0);
    }

    return 1;
}

void Ocean::release() {
    if (h_tilde)        delete[] h_tilde;
    if (h_tilde_slopex) delete[] h_tilde_slopex;
    if (h_tilde_slopez) delete[] h_tilde_slopez;
    if (h_tilde_dx)     delete[] h_tilde_dx;
    if (h_tilde_dz)     delete[] h_tilde_dz;
    if (fft)            delete fft;
    if (vertices)       delete[] vertices;
    if (indices_ln)     delete[] indices_ln;
    if (indices_tr)     delete[] indices_tr;

    glDeleteBuffers(1, &vertices_vbo);
    glDeleteBuffers(1, &indices_ln_vbo);
    glDeleteBuffers(1, &indices_tr_vbo);
    if (vao) {
        glDeleteVertexArrays(1, &vao);
    }
    Shader::releaseProgram(glProgram, glShaderV, glShaderF);
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
    Complex r = gaussianRandomVariable();
    return r * sqrt(phillips(n_prime, m_prime) / 2.0);
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

    Complex c, res, htilde_c;
    Vector2 k;
    float kx, kz, k_length, k_dot_x;

    for (int m_prime=0; m_prime<N; m_prime++) {
        kz = 2.0 * M_PI * (m_prime - N / 2.0) / length;
        for (int n_prime=0; n_prime<N; n_prime++) {
            kx = 2.0 * M_PI * (n_prime - N / 2.0) / length;
            k = Vector2(kx, kz);

            k_length = k.length();
            k_dot_x = k * x;

            c = Complex(cos(k_dot_x), sin(k_dot_x));
            htilde_c = hTilde(t, n_prime, m_prime) * c;

            h = h + htilde_c;

            n = n + Vector3(-kx * htilde_c.b, 0.0, -kz * htilde_c.b);

            if (k_length<Epsilon) continue;
            D = D + Vector2(kx / k_length * htilde_c.b, kz / k_length * htilde_c.b);
        }
    }

    n = (Vector3(0.0, 1.0, 0.0) - n).unit();

    complex_vector_norm cvn;
    cvn.h = h;
    cvn.D = D;
    cvn.n = n;
    return cvn;
}

void Ocean::evaluateWaves(float t) {
    float lambda = -1.0;
    int i;
    Vector2 x;
    Vector2 d;
    complex_vector_norm h_d_and_n;

    for (int m_prime=0; m_prime<N; m_prime++) {
        for (int n_prime=0; n_prime<N; n_prime++) {
            i = m_prime * Nplus1 + n_prime;

            x = Vector2(vertices[i].x, vertices[i].z);

            h_d_and_n = h_D_and_n(x, t);

            vertices[i].y = h_d_and_n.h.a;

            vertices[i].x = vertices[i].ox + lambda * h_d_and_n.D.x;
            vertices[i].z = vertices[i].oz + lambda * h_d_and_n.D.y;

            vertices[i].nx = h_d_and_n.n.x;
            vertices[i].ny = h_d_and_n.n.y;
            vertices[i].nz = h_d_and_n.n.z;

            if (n_prime==0 && m_prime==0) {
                vertices[i+N+Nplus1*N].y = h_d_and_n.h.a;

                vertices[i+N+Nplus1*N].x = vertices[i+N+Nplus1*N].ox + lambda*h_d_and_n.D.x;
                vertices[i+N+Nplus1*N].z = vertices[i+N+Nplus1*N].oz + lambda*h_d_and_n.D.y;

                vertices[i+N+Nplus1*N].nx = h_d_and_n.n.x;
                vertices[i+N+Nplus1*N].ny = h_d_and_n.n.y;
                vertices[i+N+Nplus1*N].nz = h_d_and_n.n.z;
            }

            if (n_prime==0) {
                vertices[i+N].y = h_d_and_n.h.a;

                vertices[i+N].x = vertices[i+N].ox + lambda*h_d_and_n.D.x;
                vertices[i+N].z = vertices[i+N].oz + lambda*h_d_and_n.D.y;

                vertices[i+N].nx = h_d_and_n.n.x;
                vertices[i+N].ny = h_d_and_n.n.y;
                vertices[i+N].nz = h_d_and_n.n.z;
            }

            if (m_prime==0) {
                vertices[i+Nplus1*N].y = h_d_and_n.h.a;

                vertices[i+Nplus1*N].x = vertices[i+Nplus1*N].ox + lambda*h_d_and_n.D.x;
                vertices[i+Nplus1*N].z = vertices[i+Nplus1*N].oz + lambda*h_d_and_n.D.y;

                vertices[i+Nplus1*N].nx = h_d_and_n.n.x;
                vertices[i+Nplus1*N].ny = h_d_and_n.n.y;
                vertices[i+Nplus1*N].nz = h_d_and_n.n.z;
            }
        }
    }
}

void Ocean::evaluateWavesFFT(float t) {
    float lambda = -1.0;
    float kx, kz, len;
    int i, i1;

    for (int m_prime=0; m_prime<N; m_prime++) {
        kz = M_PI * (2 * m_prime - N) / length;
        for (int n_prime=0; n_prime<N; n_prime++) {
            kx = M_PI * (2 * n_prime - N) / length;
            len = sqrt(kx * kx + kz * kz);
            i = m_prime * N + n_prime;

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
        fft->fft(h_tilde,        h_tilde,        1, m_prime * N);
        fft->fft(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
        fft->fft(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
        fft->fft(h_tilde_dx,     h_tilde_dx,     1, m_prime * N);
        fft->fft(h_tilde_dz,     h_tilde_dz,     1, m_prime * N);
    }
    for (int n_prime=0; n_prime<N; n_prime++) {
        fft->fft(h_tilde,        h_tilde,        N, n_prime);
        fft->fft(h_tilde_slopex, h_tilde_slopex, N, n_prime);
        fft->fft(h_tilde_slopez, h_tilde_slopez, N, n_prime);
        fft->fft(h_tilde_dx,     h_tilde_dx,     N, n_prime);
        fft->fft(h_tilde_dz,     h_tilde_dz,     N, n_prime);
    }

    int sign;
    float signs[] = {1.0, -1.0};
    Vector3 n;
    for (int m_prime=0; m_prime<N; m_prime++) {
        for (int n_prime=0; n_prime<N; n_prime++) {
            // index into h_tilde
            i  = m_prime * N      + n_prime;

            // index onto vertices
            i1 = m_prime * Nplus1 + n_prime;

            sign = signs[(n_prime + m_prime) & 1];

            h_tilde[i] = h_tilde[i] * sign;

            // height
            vertices[i1].y = h_tilde[i].a;

            // displacement
            h_tilde_dx[i] = h_tilde_dx[i] * sign;
            h_tilde_dz[i] = h_tilde_dz[i] * sign;
            vertices[i1].x = vertices[i1].ox + h_tilde_dx[i].a * lambda;
            vertices[i1].z = vertices[i1].oz + h_tilde_dz[i].a * lambda;

            // normal
            h_tilde_slopex[i] = h_tilde_slopex[i] * sign;
            h_tilde_slopez[i] = h_tilde_slopez[i] * sign;
            n = Vector3(0.0-h_tilde_slopex[i].a, 1.0, 0.0-h_tilde_slopez[i].a).unit();
            vertices[i1].nx = n.x;
            vertices[i1].ny = n.y;
            vertices[i1].nz = n.z;

            // tiling
            if (n_prime==0 && m_prime==0) {
                vertices[i1+N+Nplus1*N].y = h_tilde[i].a;

                vertices[i1+N+Nplus1*N].x = vertices[i1+N+Nplus1*N].ox + lambda*h_tilde_dx[i].a;
                vertices[i1+N+Nplus1*N].z = vertices[i1+N+Nplus1*N].oz + lambda*h_tilde_dz[i].a;

                vertices[i1+N+Nplus1*N].nx = n.x;
                vertices[i1+N+Nplus1*N].ny = n.y;
                vertices[i1+N+Nplus1*N].nz = n.z;
            }

            if (n_prime==0) {
                vertices[i1+N].y = h_tilde[i].a;

                vertices[i1+N].x = vertices[i1+N].ox + lambda*h_tilde_dx[i].a;
                vertices[i1+N].z = vertices[i1+N].oz + lambda*h_tilde_dz[i].a;

                vertices[i1+N].nx = n.x;
                vertices[i1+N].ny = n.y;
                vertices[i1+N].nz = n.z;
            }

            if (m_prime==0) {
                vertices[i1+Nplus1*N].y = h_tilde[i].a;

                vertices[i1+Nplus1*N].x = vertices[i1+Nplus1*N].ox + lambda*h_tilde_dx[i].a;
                vertices[i1+Nplus1*N].z = vertices[i1+Nplus1*N].oz + lambda*h_tilde_dz[i].a;

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

    glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ocean_vertex)*Nplus1*Nplus1, vertices);

    glm::mat4 m, mv_transp_inv;

    glUseProgram(glProgram);

    glUniform3f(uLightPos, light_pos.x, light_pos.y, light_pos.z);
    glUniformMatrix4fv(uProjection,  1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(uView,        1, GL_FALSE, glm::value_ptr(view));

    if (vao) {
        glBindVertexArray(vao);
    } else {
        initAttributes();
    }

	
    GLenum geometry            = (geometry_type==GEOMETRY_SOLID ? GL_TRIANGLES     : GL_LINES);
    GLuint indices_vbo         = (geometry_type==GEOMETRY_SOLID ? indices_tr_vbo   : indices_ln_vbo);
    unsigned int indices_count = (geometry_type==GEOMETRY_SOLID ? indices_tr_count : indices_ln_count);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);

    static const float ocean_scale = 1.f;
    for (int i=0; i<ocean_repeat; i++) {
        for (int j=0; j<ocean_repeat; j++) {
            m = glm::scale(model, glm::vec3(ocean_scale));
            m = glm::translate(m, glm::vec3(length * (-ocean_repeat/2 + i + 0.5), 0.0,
                                            length * ( ocean_repeat/2 - j - 0.5)));
            mv_transp_inv =  glm::inverse(glm::transpose(view * m));

            glUniformMatrix4fv(uModel,       1, GL_FALSE, glm::value_ptr(m));
            glUniformMatrix4fv(uMVTranspInv, 1, GL_FALSE, glm::value_ptr(mv_transp_inv));

            glDrawElements(geometry, indices_count, GL_UNSIGNED_INT, 0);
        }
    }

    if (vao) {
        glBindVertexArray(0);
    }
    glUseProgram(0);
}

void Ocean::geometryType(GEOMETRY_TYPE t) {
    geometry_type = t;
}

int Ocean::initShaderProgram() {
    if (Shader::createProgram(glProgram, glShaderV, glShaderF,
                          vertex_src_1_30, fragment_src_1_30)) {
        shaderVersion = 130;
        LOGI << "Using GLSL 1.30 for Ocean Rendering";
        return 1;
    }

    if (Shader::createProgram(glProgram, glShaderV, glShaderF,
                          vertex_src_1_10, fragment_src_1_10)) {
        shaderVersion = 110;
        LOGI << "Using GLSL 1.10 for Ocean Rendering";
        return 1;
    }

    return 0;
}

void Ocean::initAttributes() {
    glEnableVertexAttribArray(aVertex);
    glVertexAttribPointer(aVertex, 3, GL_FLOAT, GL_FALSE, sizeof(ocean_vertex), 0);

    glEnableVertexAttribArray(aNormal);
    glVertexAttribPointer(aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(ocean_vertex),
                          (void *)(sizeof(GLfloat)*3));
}
