#pragma once
#include "Hittable.hpp"
#include "utility.hpp"
#include "Material.hpp"

class Sphere : public Hittable
{

public:
    Vector3 center;
    float radius;
    shared_ptr<Material> material_ptr;
    Sphere() {}
    Sphere(const Vector3 &cen, float r, std::shared_ptr<Material> &mat)
        : center(cen), radius(r), material_ptr(mat) {}

    bool hit(const Ray &r, double t_min, double t_max, Hit_record &rec) const override
    {
        Vector3 oc = r.origin - center;
        auto a = r.direction.length_squared();
        auto half_b = oc.dot(r.direction);
        auto c = oc.length_squared() - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (discriminant > 0)
        {
            auto sqrt_d = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (-half_b - sqrt_d) / a;
            if (root < t_min || root > t_max)
            {
                root = (-half_b + sqrt_d) / a;
                if (root < t_min || root > t_max)
                    return false;
            }

            rec.t = root;
            rec.p = r.at(rec.t);
            Vector3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.material_ptr = material_ptr;
            return true;
        }
        return false;
    }


    bool bounding_box(double t0, double t1, box_ab &output_box) const
    {
        output_box = box_ab(center - Vector3(radius, radius, radius),
                          center + Vector3(radius, radius, radius));
        return true;
    }
};
