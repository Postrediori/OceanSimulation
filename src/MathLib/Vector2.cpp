// Vector.cpp
#include "stdafx.h"
#include "Vector2.h"

Vector2::Vector2() : x(0.0f), y(0.0f) { }
Vector2::Vector2(float x, float y) : x(x), y(y) { }

float Vector2::length() const {
    return sqrt(this->x*this->x + this->y*this->y);
}

Vector2 Vector2::unit() const {
    float l = length();
    if (l != 0.0f) {
        return Vector2(this->x/l, this->y/l);
    }
    return Vector2();
}

float Vector2::operator*(const Vector2& v) {
    return this->x*v.x + this->y*v.y;
}

Vector2 Vector2::operator+(const Vector2& v) {
    return Vector2(this->x+v.x, this->y+v.y);
}

Vector2 Vector2::operator-(const Vector2& v) {
    return Vector2(this->x - v.x, this->y - v.y);
}

Vector2 Vector2::operator*(const float s) {
    return Vector2(this->x*s, this->y*s);
}

Vector2& Vector2::operator=(const Vector2& v) {
    this->x = v.x;
    this->y = v.y;
    return *this;
}
