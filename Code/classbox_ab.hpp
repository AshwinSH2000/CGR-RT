#pragma once
#include "utility.hpp"

class box_ab
{
public:
    Vector3 _min;
    Vector3 _max;

    box_ab() {}
    box_ab(const Vector3 &a, const Vector3 &b)
    {
        _min = a;
        _max = b;
    }

    Vector3 min() const { return _min; }
    Vector3 max() const { return _max; }

    bool hit(const Ray &r, double tmin, double tmax) const
    {
        auto tx0 = fmin((_min.x - r.origin.x) / r.direction.x,
                        (_max.x - r.origin.x) / r.direction.x);
        auto tx1 = fmax((_min.x - r.origin.x) / r.direction.x,
                        (_max.x - r.origin.x) / r.direction.x);
        tmin = fmax(tx0, tmin);
        tmax = fmin(tx1, tmax);
        if (tmax <= tmin)
            return false;

        // this is for intersection along y axis
        auto ty0 = fmin((_min.y - r.origin.y) / r.direction.y,
                        (_max.y - r.origin.y) / r.direction.y);
        auto ty1 = fmax((_min.y - r.origin.y) / r.direction.y,
                        (_max.y - r.origin.y) / r.direction.y);
        tmin = fmax(ty0, tmin);
        tmax = fmin(ty1, tmax);
        if (tmax <= tmin)
            return false;

        // this is for intersection along y axis
        auto tz0 = fmin((_min.z - r.origin.z) / r.direction.z,
                        (_max.z - r.origin.z) / r.direction.z);
        auto tz1 = fmax((_min.z - r.origin.z) / r.direction.z,
                        (_max.z - r.origin.z) / r.direction.z);
        tmin = fmax(tz0, tmin);
        tmax = fmin(tz1, tmax);
        if (tmax <= tmin)
            return false;
        return true;
    }

    bool hit(Vector3 center)
    {

        if (_min.x > center.x || center.x > _max.x)
        {
            return false;
        }

        if (_min.y > center.y || center.y > _max.y)
        {
            return false;
        }

        if (_min.z > center.z || center.z > _max.z)
        {
            return false;
        }

        return true;
    }
};

box_ab surrounding_box(box_ab box0, box_ab box1)
{
    Vector3 small(fmin(box0.min().x, box1.min().x),
               fmin(box0.min().y, box1.min().y),
               fmin(box0.min().z, box1.min().z));

    Vector3 big(fmax(box0.max().x, box1.max().x),
             fmax(box0.max().y, box1.max().y),
             fmax(box0.max().z, box1.max().z));

    return box_ab(small, big);
}