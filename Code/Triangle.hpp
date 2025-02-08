#pragma once
#include "Hittable.hpp"
#include "utility.hpp"
#include <cmath>
#include <memory>

class Triangle : public Hittable
{
public:
    Vector3 v1, v2, v3; // Vertices of the triangle
    std::shared_ptr<Material> material_ptr;

    Triangle() {}
    
    //deafult contructor

    Triangle(const Vector3 &p1, const Vector3 &p2, const Vector3 &p3, std::shared_ptr<Material> m)
        : v1(p1), v2(p2), v3(p3), material_ptr(m) {}

    bool hit(const Ray &r, double t_min, double t_max, Hit_record &rec) const override
    {

        const double EPSILON = 1e-6;
        Vector3 edge1 = v2 - v1;
        Vector3 edge2 = v3 - v1;
        Vector3 h = r.direction.cross(edge2);
        double a = edge1.dot(h);

        // If a is close to 0, the ray is parallel to the triangle.
        if (a > -EPSILON && a < EPSILON)
            return false;

        double f = 1.0 / a;
        Vector3 s = r.origin - v1;
        double u = f * s.dot(h);

        // Check if intersection lies outside the triangle
        if (u < 0.0 || u > 1.0)
            return false;

        Vector3 q = s.cross(edge1);
        double v = f * r.direction.dot(q);

        // Check if intersection lies outside the triangle
        if (v < 0.0 || u + v > 1.0)
            return false;

        // Calculate t to find where the intersection point is on the ray
        double t = f * edge2.dot(q);

        if (t < t_min || t > t_max)
            return false;

        // Update the hit record with intersection details
        rec.t = t;
        rec.p = r.at(t);
        Vector3 outward_normal = edge1.cross(edge2).normalized();
        rec.set_face_normal(r, outward_normal);
        rec.material_ptr = material_ptr;

        return true;
    }
    bool bounding_box(double t0, double t1, box_ab &output_box) const override;
};

bool Triangle::bounding_box(double t0, double t1, box_ab &output_box) const
{
    // Compute the center and radius for the bounding sphere
    Vector3 center = (v1 + v2 + v3) / 3;
    double radius = std::max({(center - v1).length(), (center - v2).length(), (center - v3).length()});

    // Create an box_ab (Axis-Aligned Bounding Box) around the triangle
    output_box = box_ab(center - Vector3(radius, radius, radius), center + Vector3(radius, radius, radius));
    return true;
}
