// Complex.h
#pragma once

class Complex {
public:
    float a, b;
    static unsigned int additions, multiplications;

    Complex();
    Complex(float a, float b);

    Complex conj();

    Complex operator+(const Complex &c) const;
    Complex operator-(const Complex &c) const;
    Complex operator*(const Complex &c) const;
    Complex operator*(const float c) const;
    Complex &operator=(const Complex &c);

    static void resetCounters();
};

typedef std::vector<Complex> Complex_vt;
