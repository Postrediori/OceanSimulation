// Vector.h
#ifndef VECTOR_H
#define VECTOR_H

class Vector2 {
public:
    float x, y;

    Vector2();
    Vector2(float x, float y);

    float length();
    Vector2 unit();

    float operator*(const Vector2& v);
    Vector2 operator+(const Vector2& v);
    Vector2 operator-(const Vector2& v);
    Vector2 operator*(const float s);
    Vector2& operator=(const Vector2& v);
};

class Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);

    float length();
    Vector3 unit();

    float operator*(const Vector3& v);
    Vector3 operator+(const Vector3& v);
    Vector3 operator-(const Vector3& v);
    Vector3 operator*(const float s);
    Vector3& operator=(const Vector3& v);
};

#endif
