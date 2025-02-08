#pragma once
#include "Vector3.hpp"
#include <iostream>

using Color = Vector3;

void write_color(std::ostream &out, Color pixel_color, int samples_per_pixel)
{
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    out << static_cast<int>(255 * clamp(r, 0.0, 1.0)) << ' '
        << static_cast<int>(255 * clamp(g, 0.0, 1.0)) << ' '
        << static_cast<int>(255 * clamp(b, 0.0, 1.0)) << '\n';
}