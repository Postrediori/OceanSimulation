// Vector.h
#ifndef VECTOR_H
#define VECTOR_H

class Vector2 {
public:
    float x{ 0.0 }, y{ 0.0 };

    Vector2() = default;
    Vector2(float x, float y);
    Vector2(const Vector2& other);

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
    float x{ 0.0 }, y{ 0.0 }, z{ 0.0 };

    Vector3() = default;
    Vector3(const Vector3& other);
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
