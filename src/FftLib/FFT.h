// FFT.h
#pragma once

class FFT {
public:
    FFT() = default;

    void init(uint32_t N);
    void fft(const std::vector<Complex>& input,
             std::vector<Complex>& output,
             int stride, int offset);

private:
    uint32_t N = 0;
    uint32_t which = 0;
    uint32_t log_2_N = 0;
    std::vector<uint32_t> reversed;
    std::vector<std::vector<Complex>> W;
    std::vector<Complex> c[2];
};
