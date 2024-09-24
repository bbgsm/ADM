#pragma once
#include <cmath>

struct Vector3D {
    float x, y, z;

    // 构造函数
    Vector3D(float x = 0.0f, float y = 0.0f, float z = 0.0f) :
        x(x), y(y), z(z) {
    }

    // 向量加法
    Vector3D operator+(const Vector3D &v) const {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }

    // 标量乘法
    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    // 计算向量的模（长度）
    float magnitude() const {
        return sqrt(x * x + y * y + z * z);
    }

    // 计算两个向量之间的距离
    float distance(const Vector3D &v2) const {
        return (*this - v2).magnitude();
    }

    // 重载减号运算符，用于计算两个向量的差
    Vector3D operator-(const Vector3D &v) const {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }

    Vector2D toVector2D() const {
        return Vector2D(x, y);
    }

    bool isZero() const {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }
};
