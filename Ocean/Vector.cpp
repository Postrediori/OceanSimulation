// Vector.cpp
#include "stdafx.h"
#include "Vector.h"

// Vector2

Vector2::Vector2() : x(0.0f), y(0.0f) { }
Vector2::Vector2(float x, float y) : x(x), y(y) { }

float Vector2::length() {
    return sqrt(this->x*this->x + this->y*this->y);
}

Vector2 Vector2::unit() {
    float l = this->length();
    if (l!=0.0f) return Vector2(this->x/l, this->y/l);
    else return Vector2();
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

// Vector3

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) { }
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) { }

float Vector3::length() {
    return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

Vector3 Vector3::unit() {
    float l = this->length();
    if (l!=0.0f) return Vector3(this->x/l, this->y/l, this->z/l);
    else return Vector3();
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
