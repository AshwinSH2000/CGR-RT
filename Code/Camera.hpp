#pragma once
#include <cmath> // For tan and other mathematical functions
#include "utility.hpp"

class Camera
{
public:
    Vector3 origin;
    Vector3 lowerLeftCorner;
    Vector3 vertical;
    Vector3 horizontal;
    int width;
    int height;
    float exposure;
    Vector3 u, v, w;
    float camera_radius;
    Camera()
    {
    }

    Camera(const Vector3 &lookFrom, const Vector3 &lookAt, const Vector3 &up, float fov, float aspect, float aperture = 0.1, int wi = 1500, int he = 800)
        : origin(lookFrom), width(wi), height(he)
    {
        float theta = degrees_to_radians(fov);
        float halfHeight = tan(theta / 2);
        float viewport_height = 2.0 * halfHeight;
        float viewport_width = aspect * viewport_height;

        w = unit(lookFrom - lookAt);
        u = unit(w.cross(up));
        v = w.cross(u);

        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lowerLeftCorner = origin - horizontal / 2 - vertical / 2 - w;

        camera_radius = aperture / 200; 
    }

    Ray get_ray(double s, double t) const
    {
        Vector3 rd = camera_radius * random_in_unit_disk();
        Vector3 offset = u * rd.x + v * rd.y;
        return Ray(origin + offset, lowerLeftCorner + s * horizontal + t * vertical - origin - offset);
    }
    float getCameraRadius() const
    {
        return camera_radius;
    }
};