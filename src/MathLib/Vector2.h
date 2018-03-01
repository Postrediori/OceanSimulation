// Vector2.h
#pragma once

class Vector2 {
public:
    float x, y;

    Vector2();
    Vector2(float x, float y);

    float length() const;
    Vector2 unit() const;

    float operator*(const Vector2& v);
    Vector2 operator+(const Vector2& v);
    Vector2 operator-(const Vector2& v);
    Vector2 operator*(const float s);
    Vector2& operator=(const Vector2& v);
};
