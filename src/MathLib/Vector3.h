// Vector3.h
#pragma once

class Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);

    float length() const;
    Vector3 unit() const;

    float operator*(const Vector3& v);
    Vector3 operator+(const Vector3& v);
    Vector3 operator-(const Vector3& v);
    Vector3 operator*(const float s);
    Vector3& operator=(const Vector3& v);
};
