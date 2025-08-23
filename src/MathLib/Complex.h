// Complex.h
#ifndef COMPLEX_H
#define COMPLEX_H

class Complex {
public:
    float a{ 0.0 }, b{ 0.0 };
    static uint64_t additions, multiplications;

    Complex() = default;
    Complex(const Complex& other);
    Complex(float a, float b);

    Complex conj();

    Complex operator+(const Complex &c) const;
    Complex operator-(const Complex &c) const;
    Complex operator*(const Complex &c) const;
    Complex operator*(const float c) const;
    Complex &operator=(const Complex &c);

    static void resetCounters();
};

#endif
