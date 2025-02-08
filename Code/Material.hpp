#pragma once
#include "utility.hpp"
//#include "json/include/nlohmann/json.hpp"
#include "Hittable.hpp"
#include <memory>
using Color = Vector3;

class Material
{
public:
    float ks, kd, specularexponent;
    Vector3 diffusecolor, specularcolor, emissioncolor;
    bool isreflective, isrefractive;
    float reflectivity, refractiveindex;

    Material()
        : ks(0), kd(0), specularexponent(0),
          diffusecolor(Vector3(0, 0, 0)), specularcolor(Vector3(0, 0, 0)), emissioncolor(Vector3(0, 0, 0)),
          isreflective(false), isrefractive(false), reflectivity(0), refractiveindex(1.0) {}

    Material(float ks, float kd, float spec_exp, const Vector3 &diffuse, const Vector3 &specular, const Vector3 &emission,
             bool reflective, bool refractive, float reflect, float refract_index)
        : ks(ks), kd(kd), specularexponent(spec_exp),
          diffusecolor(diffuse), specularcolor(specular), emissioncolor(emission),
          isreflective(reflective), isrefractive(refractive),
          reflectivity(reflect), refractiveindex(refract_index) {}

    Material(const nlohmann::json &mat_json)
        : ks(mat_json.value("ks", 0.0f)),
          kd(mat_json.value("kd", 0.0f)),
          specularexponent(mat_json.value("specularexponent", 0.0f)),
          diffusecolor(Vector3(mat_json["diffusecolor"][0], mat_json["diffusecolor"][1], mat_json["diffusecolor"][2])),
          specularcolor(Vector3(mat_json["specularcolor"][0], mat_json["specularcolor"][1], mat_json["specularcolor"][2])),
          emissioncolor(Vector3(mat_json.value("emissioncolor", std::vector<float>{0.0, 0.0, 0.0})[0],
                             mat_json.value("emissioncolor", std::vector<float>{0.0, 0.0, 0.0})[1],
                             mat_json.value("emissioncolor", std::vector<float>{0.0, 0.0, 0.0})[2])),
          isreflective(mat_json.value("isreflective", false)),
          reflectivity(mat_json.value("reflectivity", 0.0f)),
          isrefractive(mat_json.value("isrefractive", false)),
          refractiveindex(mat_json.value("refractiveindex", 1.0f)) {}

    // Return emission color if the material is emissive
    Color emit() const
    {
        return emissioncolor;
    }

    virtual bool scatter(const Ray &rayIn, const Hit_record &rec, Vector3 &attenuation, Ray &scattered) const {}
};

class Dielectric : public Material
{
public:
    Dielectric(float refractive_index, const Vector3 &emission = Vector3(0, 0, 0))
        : Material(0, 0, 0, Vector3(1.0, 1.0, 1.0), Vector3(1.0, 1.0, 1.0), emission, false, true, 0, refractive_index) {}

    Dielectric(const nlohmann::json &mat_json)
        : Material(mat_json) {}

    virtual bool scatter(const Ray &rayIn, const Hit_record &rec, Vector3 &attenuation, Ray &scattered) const override
    {
        attenuation = Vector3(1.0, 1.0, 1.0);
        float etai_over_etat = rec.front_face ? (1.0 / refractiveindex) : refractiveindex;

        Vector3 unit_direction = unit(rayIn.direction);
        float cos_theta = std::fmin(-unit_direction.dot(rec.normal), 1.0);
        float sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        if (etai_over_etat * sin_theta > 1.0)
        {
            Vector3 reflected = reflect(unit_direction, rec.normal);
            scattered = Ray(rec.p, reflected);
            return true;
        }

        float reflect_prob = schlick(cos_theta, etai_over_etat);
        if (random_double() < reflect_prob)
        {
            Vector3 reflected = reflect(unit_direction, rec.normal);
            scattered = Ray(rec.p, reflected);
            return true;
        }

        Vector3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
        scattered = Ray(rec.p, refracted);
        return true;
    }
};
// Sample Material Types with Emission Support
class Diffuse : public Material
{
public:
    Diffuse(const Vector3 &albedo, const Vector3 &emission = Vector3(0, 0, 0))
        : Material(0, 1, 0, albedo, Vector3(0, 0, 0), emission, false, false, 0, 1.0) {}

    Diffuse(const nlohmann::json &mat_json)
        : Material(mat_json) {}

    virtual bool scatter(const Ray &rayIn, const Hit_record &rec, Vector3 &attenuation, Ray &scattered) const override
    {
        Vector3 scatter_direction = rec.normal + random_unit_vector();
        scattered = Ray(rec.p, scatter_direction);
        attenuation = diffusecolor;
        return true;
    }
};

class Metal : public Material
{
public:
    float fuzz;

    Metal(const Vector3 &albedo, float f, const Vector3 &emission = Vector3(0, 0, 0))
        : Material(1, 0, 0, albedo, albedo, emission, true, false, 1.0, 1.0), fuzz(f < 1 ? f : 1) {}

    Metal(const nlohmann::json &mat_json)
        : Material(mat_json), fuzz(mat_json.value("fuzz", 0.0f)) {}

    virtual bool scatter(const Ray &rayIn, const Hit_record &rec, Vector3 &attenuation, Ray &scattered) const override
    {
        Vector3 reflected = reflect(unit(rayIn.direction), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = diffusecolor;
        return (scattered.direction.dot(rec.normal) > 0);
    }
};
