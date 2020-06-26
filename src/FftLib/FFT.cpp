// FFT.h
#include "stdafx.h"
#include "Complex.h"
#include "FFT.h"

static const double Pi2 = 2.0 * M_PI;

unsigned int log_2(unsigned int N) {
    unsigned int r = 0;
    unsigned int k = N;
    while (k >>= 1) {
        r++;
    }
    return r;
}

// Reverse bits in i
unsigned int reverse(unsigned int i, unsigned int log_2_N) {
    unsigned int res = 0;
    for (unsigned int j = 0; j < log_2_N; j++) {
        res = (res << 1) + (i & 1);
        i >>= 1;
    }
    return res;
}

// Produce array of reversed indices
std::vector<unsigned int> reversedArray(unsigned int N) {
    unsigned int log_2_N = log_2(N);
    std::vector<unsigned int> reversed(N);
    
    for (unsigned int i = 0; i < N; i++) {
        reversed[i] = reverse(i, log_2_N);
    }

    return reversed;
}

// Division of a full circle into N parts
Complex FftW(unsigned int x, unsigned int N) {
    return Complex(cos(Pi2 * x / N), sin(Pi2 * x / N));
}

void FFT::init(unsigned int N_) {
    N = N_;
    log_2_N = log_2(N);

    // prepare bit reversals
    reversed = reversedArray(N);

    // prepare W
    unsigned int pow2 = 1;
    W.resize(log_2_N);
    for (unsigned int i=0; i<log_2_N; i++) {
        W[i].resize(pow2);
        for (unsigned int j = 0; j < pow2; j++) {
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

    for (unsigned int i = 0; i < N; i++) {
        c[which][i] = input[reversed[i] * stride + offset];
    }

    int loops = N >> 1;
    int size = 1 << 1;
    int size_over_2 = 1;
    int w_ = 0;

    for (unsigned int i = 1; i <= log_2_N; i++) {
        which ^= 1;
        for (int j = 0; j < loops; j++) {
            for (int k = 0; k < size_over_2; k++) {
                c[which][size*j + k] = c[which ^ 1][size*j + k] +
                    c[which ^ 1][size*j + size_over_2 + k] * W[w_][k];
            }
            for (int k = size_over_2; k < size; k++) {
                c[which][size*j + k] = c[which ^ 1][size*j - size_over_2 + k] -
                    c[which ^ 1][size*j + k] * W[w_][k - size_over_2];
            }
        }

        loops >>= 1;
        size <<= 1;
        size_over_2 <<= 1;
        w_++;
    }

    for (unsigned int i = 0; i < N; i++) {
        output[i*stride + offset] = c[which][i];
    }
}
