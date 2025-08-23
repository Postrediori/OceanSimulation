// FFT.h
#include "stdafx.h"
#include "Complex.h"
#include "FFT.h"

constexpr double Tau = 2.0 * M_PI;

uint32_t log_2(uint32_t N) {
    uint32_t r = 0;
    uint32_t k = N;
    while (k >>= 1) {
        r++;
    }
    return r;
}

// Reverse bits in i
uint32_t reverse(uint32_t i, uint32_t log_2_N) {
    uint32_t res = 0;
    for (uint32_t j = 0; j < log_2_N; j++) {
        res = (res << 1) + (i & 1);
        i >>= 1;
    }
    return res;
}

// Produce array of reversed indices
std::vector<uint32_t> reversedArray(uint32_t N) {
    uint32_t log_2_N = log_2(N);
    std::vector<uint32_t> reversed(N);

    for (uint32_t i = 0; i < N; i++) {
        reversed[i] = reverse(i, log_2_N);
    }

    return reversed;
}

// Division of a full circle into N parts
Complex FftW(uint32_t x, uint32_t N) {
    const double angle = Tau * x / N;
    return Complex(cos(angle), sin(angle));
}

void FFT::init(uint32_t N_) {
    N = N_;
    log_2_N = log_2(N);

    // prepare bit reversals
    reversed = reversedArray(N);

    // prepare W
    uint32_t pow2 = 1;
    W.resize(log_2_N);
    for (uint32_t i=0; i<log_2_N; i++) {
        W[i].resize(pow2);
        for (uint32_t j = 0; j < pow2; j++) {
            W[i][j] = FftW(j, pow2 * 2);
        }
        pow2 *= 2;
    }

    c[0].resize(N);
    c[1].resize(N);
    which = 0;
}

void FFT::fft(const std::vector<Complex>& input,
    std::vector<Complex>& output,
    int stride, int offset) {

    for (size_t i = 0; i < N; i++) {
        c[which][i] = input[reversed[i] * stride + offset];
    }

    uint32_t loops = N >> 1;
    uint32_t size = 1 << 1;
    uint32_t size_over_2 = 1;
    uint32_t w_ = 0;

    for (uint32_t i = 1; i <= log_2_N; i++) {
        which ^= 1;
        for (uint32_t j = 0; j < loops; j++) {
            for (uint32_t k = 0; k < size_over_2; k++) {
                c[which][size*j + k] = c[which ^ 1][size*j + k] +
                    c[which ^ 1][size*j + size_over_2 + k] * W[w_][k];
            }
            for (uint32_t k = size_over_2; k < size; k++) {
                c[which][size*j + k] = c[which ^ 1][size*j - size_over_2 + k] -
                    c[which ^ 1][size*j + k] * W[w_][k - size_over_2];
            }
        }

        loops >>= 1;
        size <<= 1;
        size_over_2 <<= 1;
        w_++;
    }

    for (size_t i = 0; i < N; i++) {
        output[i*stride + offset] = c[which][i];
    }
}
