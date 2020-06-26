// FFT.h
#pragma once

class FFT {
public:
    void init(unsigned int N);
    void fft(const std::vector<Complex>& input,
            std::vector<Complex>& output,
             int stride, int offset);

private:
    unsigned int N = 0;
    unsigned int which = 0;
    unsigned int log_2_N = 0;
    std::vector<unsigned int> reversed;
    std::vector<std::vector<Complex>> W;
    std::vector<Complex> c[2];
};
