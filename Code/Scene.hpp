#pragma once
#include <vector>
#include <memory>
#include "Ray.hpp"
#include "Hittable.hpp"
#include "Light.hpp"

class Scene
{
public:
    std::vector<std::shared_ptr<Hittable>> objects; // vector of all hittable objects in the scene
    std::vector<Light> lights;                      // vector of all light sources in the scene
    Scene() = default;
    
    // Add light sources to the scene
    void addLight(const Light &light)
    {
        lights.push_back(light);
    }
    // Add objects (spheres, triangles, cylinders ) to the scene
    void addObject(std::shared_ptr<Hittable> object)
    {
        objects.push_back(object);
    }

    

    // Check for intersections with objects in the scene
    bool hit(const Ray &r, double t_min, double t_max, Hit_record &rec) const
    {
        Hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        // Iterate through all objects and check for hits
        for (const auto &object : objects)
        {
            if (object->hit(r, t_min, closest_so_far, temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    // Access lights in the scene
    const std::vector<Light> &getLights() const
    {
        return lights;
    }
    
};