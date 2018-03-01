// Vector3.cpp
#include "stdafx.h"
#include "Vector3.h"

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) { }
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) { }

float Vector3::length() const {
    return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

Vector3 Vector3::unit() const {
    float l = length();
    if (l != 0.0f) {
        return Vector3(this->x/l, this->y/l, this->z/l);
    }
    return Vector3();
}

float Vector3::operator*(const Vector3& v) {
    return this->x*v.x + this->y*v.y + this->z*v.z;
}

Vector3 Vector3::operator+(const Vector3& v) {
    return Vector3(this->x + v.x, this->y + v.y, this->z + v.z);
}

Vector3 Vector3::operator-(const Vector3& v) {
    return Vector3(this->x - v.x, this->y - v.y, this->z - v.z);
}

Vector3 Vector3::operator*(const float s) {
    return Vector3(this->x*s, this->y*s, this->z*s);
}

Vector3& Vector3::operator=(const Vector3& v) {
    this->x = v.x;
    this->y = v.y;
    this->z = v.z;
    return *this;
}
