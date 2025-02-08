#pragma once
#include <cmath>
#include <iostream>
#include "json/include/nlohmann/json.hpp"
#include "utility.hpp"

class Vector3
{
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3(const nlohmann::json &json_ary)
        : x(json_ary.at(0).get<float>()),
          y(json_ary.at(1).get<float>()),
          z(json_ary.at(2).get<float>()) {}
    inline float getX() const { return x; }
    inline float getY() const { return y; }
    inline float getZ() const { return z; }
    Vector3 operator+(const Vector3 &v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    Vector3 operator-() const
    {
        return Vector3(-x, -y, -z);
    }

    Vector3 &operator+=(const Vector3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector3 operator-(const Vector3 &v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 operator*(float scalar) const
    {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 &operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    Vector3 operator*(const Vector3 &other) const
    {
        return Vector3(x * other.x, y * other.y, z * other.z);
    }

    Vector3 &operator*=(const Vector3 &other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }

    Vector3 operator/(float scalar) const
    {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    Vector3 &operator/=(const Vector3 &other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    Vector3 &operator/=(const double t)
    {
        return *this *= 1 / t;
    }

    double length_squared() const
    {
        return x * x + y * y + z * z;
    }

    double length() const
    {
        return sqrt(length_squared());
    }

    Vector3 cross(const Vector3 &v) const
    {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    float dot(const Vector3 &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 normalized() const
    {
        float len = sqrt(x * x + y * y + z * z);
        return *this / len;
    }
    inline static Vector3 random()
    {
        return Vector3(random_double(), random_double(), random_double());
    }

    inline static Vector3 random(double min, double max)
    {
        return Vector3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
};
inline std::ostream &operator<<(std::ostream &out, const Vector3 &v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z << std::endl;
}

inline Vector3 unit(const Vector3 &v)
{
    return v / v.length();
}

Vector3 random_in_unit_sphere()
{
    while (true)
    {
        auto p = Vector3::random(-1, 1);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

Vector3 random_in_unit_disk()
{
    while (true)
    {
        auto p = Vector3(random_double(-1, 1), random_double(-1, 1), 0);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

Vector3 random_unit_vector()
{
    auto a = random_double(0, 2 * pi);
    auto z = random_double(-1, 1);
    auto r = sqrt(1 - z * z);
    return Vector3(r * cos(a), r * sin(a), z);
}

Vector3 random_in_hemisphere(const Vector3 &normal)
{
    Vector3 in_unit_sphere = random_in_unit_sphere();
    if (in_unit_sphere.dot(normal) > 0.0)
    {
        // In the same hemisphere as the normal.
        return in_unit_sphere;
    }
    else
    {
        return -in_unit_sphere;
    }
}

Vector3 reflect(const Vector3 &v, const Vector3 &n)
{
    return v - n * 2 * v.dot(n);
}

inline Vector3 operator*(double t, const Vector3 &v)
{
    return Vector3(t * v.x, t * v.y, t * v.z);
}

Vector3 refract(const Vector3 &uv, const Vector3 &n, double etai_over_etat)
{
    auto cos_theta = (-uv).dot(n);
    Vector3 r_out_paralell = etai_over_etat * (uv + cos_theta * n);
    Vector3 r_out_perp = -sqrt(1.0 - r_out_paralell.length_squared()) * n;
    return r_out_paralell + r_out_perp;
}
