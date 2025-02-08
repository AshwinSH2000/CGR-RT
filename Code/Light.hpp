#pragma once
#include "Ray.hpp"

// Define a simple point light class
class Light
{
public:
    Vector3 position;  // Position of the light in the scene
    Vector3 intensity; // Intensity of the light

    Light(const Vector3 &pos, const Vector3 &intensity)
        : position(pos), intensity(intensity) {}
};