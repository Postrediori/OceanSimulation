// FFT.h
#include "stdafx.h"
#include "Complex.h"
#include "FFT.h"

FFT::FFT(unsigned int N)
    : N(N)
    , pi2(2.0 * M_PI)
    , reversed(0)
    , W(nullptr) {
    c[0] = c[1] = nullptr;

    log_2_N = (unsigned int)(log((double)N) / log((double)2));

    // prepare bit reversals
    reversed = new unsigned int[N];
    for (int i = 0; i < N; i++) {
        reversed[i] = reverse(i);
    }

    // prepare W
    int pow2 = 1;
    W = new Complex*[log_2_N];
    for (int i=0; i<log_2_N; i++) {
        W[i] = new Complex[pow2];
        for (int j = 0; j < pow2; j++) {
            W[i][j] = w(j, pow2 * 2);
        }
        pow2 *= 2;
    }

    c[0] = new Complex[N];
    c[1] = new Complex[N];
    which = 0;
}

FFT::~FFT() {
    if (c[0]) {
        delete[] c[0];
    }
    if (c[1]) {
        delete[] c[1];
    }
    if (W) {
        for (int i = 0; i < log_2_N; i++) {
            if (W[i]) {
                delete[] W[i];
            }
        }
        delete[] W;
    }
    if (reversed) {
        delete[] reversed;
    }
}

unsigned int FFT::reverse(unsigned int i)	{
    unsigned int res = 0;
    for (int j=0; j<log_2_N; j++) {
        res = (res<<1) + (i & 1);
        i >>= 1;
    }
    return res;
}

Complex FFT::w(unsigned int x, unsigned int N) {
    return Complex(cos(pi2 * x / N), sin(pi2 * x / N));
}

void FFT::fft(Complex *input, Complex *output,
    int stride, int offset) {
    for (int i = 0; i < N; i++) {
        c[which][i] = input[reversed[i] * stride + offset];
    }

    int loops = N >> 1;
    int size = 1 << 1;
    int size_over_2 = 1;
    int w_ = 0;

    for (int i = 1; i <= log_2_N; i++) {
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

    for (int i = 0; i < N; i++) {
        output[i*stride + offset] = c[which][i];
    }
}
