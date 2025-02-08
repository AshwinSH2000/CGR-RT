#pragma once
#include "Hittable.hpp"
#include "utility.hpp"
#include <cmath>
#include <memory>

class Cylinder : public Hittable
{
public:
    Vector3 center; // Center point of the cylinder (midpoint of the axis)
    Vector3 axis;   // Normalized axis of the cylinder (from base to top)
    double radius;
    double height;
    std::shared_ptr<Material> material_ptr;

    Cylinder() {}

    Cylinder(const Vector3 &c, const Vector3 &a, double r, double h, std::shared_ptr<Material> m)
        : center(c), axis(a.normalized()), radius(r), height(h), material_ptr(m) {}

    bool hit(const Ray &r, double t_min, double t_max, Hit_record &rec) const override
    {
        // Define the top and bottom centers based on the full height along the axis
        Vector3 base_center = center - height * axis;
        Vector3 top_center = center + height * axis;

        // Intersection with curved surface of the cylinder
        Vector3 oc = r.origin - base_center;
        Vector3 w = axis * axis.dot(r.direction); // Parallel component along axis
        Vector3 d = r.direction - w;              // Perpendicular component to axis
        Vector3 v = oc - axis * axis.dot(oc);     // Projected origin onto the plane

        double a = d.dot(d);
        double half_b = v.dot(d);
        double c = v.dot(v) - radius * radius;
        double discriminant = half_b * half_b - a * c;

        if (discriminant > 0)
        {
            double sqrt_d = std::sqrt(discriminant);
            double t = (-half_b - sqrt_d) / a;

            if (t < t_min || t > t_max)
            {
                t = (-half_b + sqrt_d) / a;
                if (t < t_min || t > t_max)
                    return false;
            }

            Vector3 hit_point = r.at(t);
            double projection = (hit_point - base_center).dot(axis);

            if (projection >= 0 && projection <= 2 * height)
            {
                rec.t = t;
                rec.p = hit_point;
                rec.normal = ((hit_point - base_center) - axis * projection).normalized();
                rec.material_ptr = material_ptr;
                return true;
            }
        }

        // Check intersection with the bottom cap
        if (intersect_caps(r, t_min, t_max, rec, base_center, false))
            return true;

        // Check intersection with the top cap
        if (intersect_caps(r, t_min, t_max, rec, top_center, true))
            return true;

        return false;
    }

    virtual bool bounding_box(double t0, double t1, box_ab &output_box) const override
    {
        Vector3 base_center = center - height * axis;
        Vector3 top_center = center + height * axis;

        // Calculate min and max bounds by expanding along cylinder's radius
        Vector3 min_bound(
            std::fmin(base_center.x - radius, top_center.x - radius),
            std::fmin(base_center.y - radius, top_center.y - radius),
            std::fmin(base_center.z - radius, top_center.z - radius));
        Vector3 max_bound(
            std::fmax(base_center.x + radius, top_center.x + radius),
            std::fmax(base_center.y + radius, top_center.y + radius),
            std::fmax(base_center.z + radius, top_center.z + radius));

        output_box = box_ab(min_bound, max_bound);
        return true;
    }

    bool intersect_caps(const Ray &r, double t_min, double t_max, Hit_record &rec, const Vector3 &cap_center, bool is_top) const
    {
        // Project ray direction onto the cylinder's axis to find intersection with cap plane
        double denom = r.direction.dot(axis);
        if (std::fabs(denom) > 1e-6)
        { // Avoid division by zero, ray parallel to cap plane hence wont intersect
            double t = (cap_center - r.origin).dot(axis) / denom;
            if (t < t_min || t > t_max)
                return false;

            Vector3 point = r.at(t);
            double dist_from_center = (point - cap_center).length();

            if (dist_from_center <= radius)
            {
                rec.t = t;
                rec.p = point;
                rec.normal = is_top ? axis : -axis; // Normal points outwards from the cap
                rec.material_ptr = material_ptr;
                return true;
            }
        }
        return false;
    }
};
