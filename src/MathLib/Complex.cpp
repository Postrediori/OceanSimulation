// Complex.cpp
#include "stdafx.h"
#include "Complex.h"

unsigned int Complex::additions = 0;
unsigned int Complex::multiplications = 0;

Complex::Complex() : a(0.0), b(0.0) { }
Complex::Complex(float a, float b) : a(a), b(b) { }

Complex Complex::conj() {
    return Complex(this->a, -this->b);
}

Complex Complex::operator+(const Complex& c) const {
    Complex::additions++;
    return Complex(this->a + c.a, this->b + c.b);
}

Complex Complex::operator-(const Complex& c) const {
    Complex::additions++;
    return Complex(this->a - c.a, this->b - c.b);
}

Complex Complex::operator*(const Complex& c) const {
    Complex::multiplications++;
    return Complex(this->a*c.a - this->b*c.b, this->a*c.b + this->b*c.a);
}

Complex Complex::operator*(const float c) const {
    return Complex(this->a * c, this->b * c);
}

Complex& Complex::operator=(const Complex &c) {
    this->a = c.a;
    this->b = c.b;
    return *this;
}

void Complex::resetCounters() {
    additions = 0;
    multiplications = 0;
}
