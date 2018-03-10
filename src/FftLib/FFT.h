// FFT.h
#ifndef FFT_H
#define FFT_H

class FFT {
public:
    FFT(unsigned int N);

    unsigned int reverse(unsigned int i) const;
    Complex w(unsigned int x, unsigned int N) const;
    void fft(const Complex_vt& input, Complex_vt& output,
             int stride, int offset);

private:
    unsigned int N, which;
    unsigned int log_2_N;
    float pi2;
    std::vector<unsigned int> reversed;
    std::vector<Complex_vt> W;
    Complex_vt c[2];
};

#endif
