#pragma once
#include "classbox_ab.hpp"

class Material;

class Hit_record
{
public:
  Vector3 p;
  Vector3 normal;
  shared_ptr<Material> material_ptr;
  double t;
  bool front_face;

  inline void set_face_normal(const Ray &r, const Vector3 &outward_normal)
  {
    front_face = r.direction.dot(outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hittable
{
public:
  virtual bool hit(const Ray &r, double t_min, double t_max, Hit_record &rec) const = 0;
  virtual bool bounding_box(double t0, double t1, box_ab &output_box) const = 0;

public:
  Vector3 center = Vector3(0, 0, 0);
};