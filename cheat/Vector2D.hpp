#pragma once
#include <cmath>
#include <stdexcept>

struct Vector2D {
    float x, y;

    // 构造函数
    Vector2D(float x = 0.0f, float y = 0.0f) :
        x(x), y(y) {
    }

    // 向量加法
    Vector2D operator+(const Vector2D &v) const {
        return Vector2D(x + v.x, y + v.y);
    }

    // 标量乘法
    Vector2D operator*(float scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    // 向量减法
    Vector2D operator-(const Vector2D &v) const {
        return Vector2D(x - v.x, y - v.y);
    }

    // 计算向量的模（长度）
    float magnitude() const {
        return sqrtf(x * x + y * y);
    }

    // 计算两个向量之间的距离
    float distance(const Vector2D &v2) const {
        return (*this - v2).magnitude();
    }

};
