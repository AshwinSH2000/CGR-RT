#pragma once
#include "Vector3.hpp"
class Ray
{
public:
    Vector3 origin;
    Vector3 direction;

    Ray() {}
    //default contr with no values

    
    Ray(const Vector3 &origin, const Vector3 &direction) : origin(origin), direction(direction) {}

    Vector3 at(float t) const
    {
        return origin + direction * t;
    }
};