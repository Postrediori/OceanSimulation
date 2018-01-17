// FFT.h
#ifndef FFT_H
#define FFT_H

class FFT {
public:
    FFT(unsigned int N);
    ~FFT();

    unsigned int reverse(unsigned int i);
    Complex w(unsigned int x, unsigned int N);
    void fft(Complex *input, Complex *output,
             int stride, int offset);

private:
    unsigned int N, which;
    unsigned int log_2_N;
    float pi2;
    unsigned int *reversed;
    Complex **W;
    Complex *c[2];
};

#endif
