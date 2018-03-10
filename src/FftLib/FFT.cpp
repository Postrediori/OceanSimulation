// FFT.h
#include "stdafx.h"
#include "Complex.h"
#include "FFT.h"

static unsigned int log2(unsigned int n) {
    return (unsigned int)(log((double)n) / log((double)2));
}

FFT::FFT(unsigned int N)
    : N(N)
    , pi2(2.0 * M_PI) {
    log_2_N = log2(N);

    // prepare bit reversals
    reversed.resize(N);
    for (unsigned int i = 0; i < N; i++) {
        reversed[i] = reverse(i);
    }

    // prepare W
    unsigned int pow2 = 1;
    W.resize(log_2_N);
    for (unsigned int i = 0; i < log_2_N; i++) {
        W[i].resize(pow2);
        for (unsigned int j = 0; j < pow2; j++) {
            W[i][j] = w(j, pow2 * 2);
        }
        pow2 *= 2;
    }

    c[0].resize(N);
    c[1].resize(N);
    which = 0;
}

unsigned int FFT::reverse(unsigned int i) const {
    unsigned int res = 0;
    for (unsigned int j = 0; j < log_2_N; j++) {
        res = (res << 1) + (i & 1);
        i >>= 1;
    }
    return res;
}

Complex FFT::w(unsigned int x, unsigned int N) const {
    return Complex(cos(pi2 * x / N), sin(pi2 * x / N));
}

void FFT::fft(const Complex_vt& input, Complex_vt& output,
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
