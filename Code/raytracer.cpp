#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "json/include/nlohmann/json.hpp"
#include "BVH.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "Cylinder.hpp"
#include "Texture.hpp" //custom texture class`
#include "Material.hpp"
#include "Hittable.hpp"
#include "HitRecord.hpp"
#include "utility.hpp"
// #include "Vector2.hpp"   //used for textures
#include <future>

using Color = Vector3;
using json = nlohmann::json;

Vector3 barycentric(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c)
{
    Vector3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0.dot(v0);
    float d01 = v0.dot(v1);
    float d11 = v1.dot(v1);
    float d20 = v2.dot(v0);
    float d21 = v2.dot(v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    return Vector3(u, v, w);
}

Color textureMappingTriangle(const Vector3 &p, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector2 &ta, const Vector2 &tb, const Vector2 &tc, const Texture &texture)
{
    Vector3 bary = barycentric(p, a, b, c);
    Vector2 texCoord = ta * bary.x + tb * bary.y + tc * bary.z;
    return texture.sample(texCoord);
}

void write_color(std::ostream &out, Color pixel_color, int samples_per_pixel, float exposure)
{
    auto r = pixel_color.x;
    auto g = pixel_color.y;
    auto b = pixel_color.z;

    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255 * clamp(r, 0.0, 1.0)) << ' '
        << static_cast<int>(255 * clamp(g, 0.0, 1.0)) << ' '
        << static_cast<int>(255 * clamp(b, 0.0, 1.0)) << '\n';
}

Camera parseCamera(const json &j)
{
    auto cam_data = j["camera"];
    return Camera(Vector3(cam_data["position"]),
                  Vector3(cam_data["lookAt"]),
                  Vector3(cam_data["upVector"]),
                  cam_data["fov"].get<float>(),
                  static_cast<float>(cam_data["width"].get<int>()) / cam_data["height"].get<int>(),
                  cam_data["exposure"].get<float>(),
                  cam_data["width"].get<int>(),
                  cam_data["height"].get<int>());
}

void parseLights(const json &j, std::vector<Light> &lights)
{
    if (j["scene"].contains("lightsources"))
    {
        for (const auto &light : j["scene"]["lightsources"])
        {
            Vector3 position(light["position"]);
            Color intensity(light["intensity"]);
            lights.emplace_back(position, intensity);
        }
    }
}

void parseScene(const json &j, std::vector<std::shared_ptr<Hittable>> &objects)
{
    for (const auto &obj : j["scene"]["shapes"])
    {
        std::shared_ptr<Material> material = std::make_shared<Diffuse>(Vector3(1, 0, 0));

        if (obj.contains("material"))
        {
            const auto &mat_json = obj["material"];
            if (mat_json.contains("isrefractive") && mat_json["isrefractive"].get<bool>())
            {
                material = std::make_shared<Dielectric>(obj["material"]);
            }
            else if (mat_json.contains("isreflective") && mat_json["isreflective"].get<bool>())
            {
                material = std::make_shared<Metal>(obj["material"]);
            }
            else
            {
                material = std::make_shared<Diffuse>(obj["material"]);
            }
        }

        if (obj["type"] == "sphere")
        {
            objects.push_back(std::make_shared<Sphere>(
                Vector3(obj["center"]),
                obj["radius"].get<float>(),
                material));
        }
        else if (obj["type"] == "cylinder")
        {
            objects.push_back(std::make_shared<Cylinder>(
                Vector3(obj["center"]),
                Vector3(obj["axis"]),
                obj["radius"].get<float>(),
                obj["height"].get<float>(),
                material));
        }
        else if (obj["type"] == "triangle")
        {
            objects.push_back(std::make_shared<Triangle>(
                Vector3(obj["v0"]),
                Vector3(obj["v1"]),
                Vector3(obj["v2"]),
                material));
        }
    }
}

Color linearToneMapping(const Color &color, float exposure)
{
    Color mapped = color * exposure; // Scale based on exposure

    // Clamp to [0, 1] to prevent tehm from going out of range
    return Color(
        fmin(mapped.x, 1.0f),
        fmin(mapped.y, 1.0f),
        fmin(mapped.z, 1.0f));
}

void parseScene(const json &j, hittable_list &world)
{
    for (const auto &obj : j["scene"]["shapes"])
    {
        std::shared_ptr<Material> material = std::make_shared<Diffuse>(Vector3(1, 0, 0));

        if (obj.contains("material"))
        {
            const auto &mat_json = obj["material"];
            if (mat_json.contains("isrefractive") && mat_json["isrefractive"].get<bool>())
            {
                material = std::make_shared<Dielectric>(obj["material"]);
            }
            else if (mat_json.contains("isreflective") && mat_json["isreflective"].get<bool>())
            {
                material = std::make_shared<Metal>(obj["material"]);
            }
            else
            {
                material = std::make_shared<Diffuse>(obj["material"]);
            }
        }
        else
        {
            // No material specified, use the default material
            material = std::make_shared<Diffuse>(Vector3(1, 0, 0)); // Red Diffuse material
        }

        if (obj.contains("type") && obj["type"] == "sphere" && obj.contains("center") && obj.contains("radius"))
        {
            world.add(std::make_shared<Sphere>(
                Vector3(obj["center"]),
                obj["radius"].get<float>(),
                material));
        }
        else if (obj.contains("type") && obj["type"] == "cylinder" && obj.contains("center") && obj.contains("axis") &&
                 obj.contains("radius") && obj.contains("height"))
        {
            world.add(std::make_shared<Cylinder>(
                Vector3(obj["center"]),
                Vector3(obj["axis"]),
                obj["radius"].get<float>(),
                obj["height"].get<float>(),
                material));
        }
        else if (obj.contains("type") && obj["type"] == "triangle" &&
                 obj.contains("v0") && obj.contains("v1") && obj.contains("v2"))
        {

            world.add(std::make_shared<Triangle>(
                Vector3(obj["v0"]),
                Vector3(obj["v1"]),
                Vector3(obj["v2"]),
                material));
        }
    }
}

Color blinn_phong_shading(const Vector3 &view_dir, const Vector3 &light_dir, const Vector3 &normal, const Material &material, const Color &light_intensity)
{
    // Ambient component
    Color ambient = 0.1 * material.diffusecolor; // Adjust ambient factor as needed

    // Specular component
    Vector3 halfway_dir = (view_dir + light_dir).normalized();
    float spec = std::pow(std::max(0.0f, normal.dot(halfway_dir)), material.specularexponent);
    Color specular = spec * material.ks * material.specularcolor * light_intensity;

    // Diffuse component
    float diff = std::max(0.0f, normal.dot(light_dir));
    Color diffuse = diff * material.kd * material.diffusecolor * light_intensity;

    // Combine all components
    return ambient + specular + diffuse;
}

Color lerp(const Color &a, const Color &b, float t)
{
    return a * (1 - t) + b * t;
}

Color Binary_Ray_Color(const Ray &r, const BVHNode &world, const Color &background_color)
{
    Hit_record rec;
    if (world.hit(r, 0.001, inf, rec))
    {
        Color lighting(1, 0, 0);
        return lighting;
    }

    return background_color;
}

Color ray_color_phong(const Ray &r, const BVHNode &world, const std::vector<Light> &lights, const Color &background_color, int depth)
{
    if (depth <= 0)
        return Color(0, 0, 0);

    Hit_record rec;
    if (world.hit(r, 0.001, inf, rec))
    {
        Color lighting(0.1, 0.1, 0.1);
        Vector3 view_dir = -r.direction.normalized();

        // code for texture that is not working.
        // Color texColor = rec.material_ptr->texture->sample(rec.uv);

        for (const auto &light : lights)
        {
            Vector3 light_dir = (light.position - rec.p).normalized();
            Ray shadow_ray(rec.p, light_dir);
            Hit_record shadow_rec;

            if (!world.hit(shadow_ray, 0.001, (light.position - rec.p).length(), shadow_rec))
            {
                // Use the blinn_phong_shading function for each light
                lighting += blinn_phong_shading(view_dir, light_dir, rec.normal, *rec.material_ptr, light.intensity);
            }
        }

        // Reflection handling
        if (rec.material_ptr->isreflective && depth > 0)
        {
            Vector3 reflected_dir = reflect(r.direction.normalized(), rec.normal);
            Ray reflected_ray(rec.p, reflected_dir);

            float cos_theta = std::max(-reflected_dir.dot(rec.normal), 0.0f);
            float fresnel = rec.material_ptr->reflectivity + (1.0f - rec.material_ptr->reflectivity) * std::pow(1.0f - cos_theta, 5);

            Color reflected_color = ray_color_phong(reflected_ray, world, lights, background_color, depth - 1);
            lighting = lerp(lighting, reflected_color, fresnel);
        }

        return lighting;
        // return texColor * lighting;  code for texture that is not working
    }

    return background_color;
}

void render_image(std::vector<Color> &framebuffer, Camera &camera, const BVHNode &world, const std::vector<Light> &lights, const Color &background_color, int width, int height, int samples_per_pixel, int max_depth, int TraceType)
{
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            Color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s)
            {
                float u = (x + random_double()) / (width - 1);
                float v = (y + random_double()) / (height - 1);
                Ray ray = camera.get_ray(u, v);

                if (TraceType == 1)
                {
                    pixel_color += Binary_Ray_Color(ray, world, background_color);
                }
                else if (TraceType == 2)
                {
                    pixel_color += ray_color_phong(ray, world, lights, background_color, max_depth);
                }
            }
            framebuffer[y * width + x] = pixel_color;
        }
    }
}

std::future<std::vector<Light>> async_parseLights(const json &j)
{
    return std::async(std::launch::async, [](const json &j)
                      {
        std::vector<Light> lights;
        parseLights(j, lights);
        return lights; }, j);
}

std::future<Camera> async_parseCamera(const json &j)
{
    return std::async(std::launch::async, parseCamera, j);
}

std::future<hittable_list> async_parseScene(const json &j)
{
    return std::async(std::launch::async, [](const json &j)
                      {
        hittable_list world;
        parseScene(j, world);
        return world; }, j);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Input the JSON file too..." << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    json j;
    file >> j;

    auto camera_future = async_parseCamera(j);
    auto lights_future = async_parseLights(j);
    auto scene_future = async_parseScene(j);

    Camera camera = camera_future.get();
    hittable_list world = scene_future.get();
    std::vector<Light> lights = lights_future.get();

    std::vector<std::shared_ptr<Hittable>> objects;
    parseScene(j, objects);

    BVHNode bvh_tree(objects, 0, objects.size(), 0.0, 0);

    Color background_color = j["scene"].contains("backgroundcolor") ? Color(j["scene"]["backgroundcolor"]) : Color(0.25, 0.25, 0.25);

    std::cout << "Press 1 for Binary-RayTracing, 2 for Blinn-Phong-RayTracing: ";
    int TraceType;
    std::cin >> TraceType;
    std::cout << "\n\nRendering...";
    std::cout << "\n\r";
    int width = j["camera"]["width"];
    int height = j["camera"]["height"];
    int samples_per_pixel = 10;
    int max_depth = 5;
    std::vector<Color> framebuffer(width * height);
    auto start = std::chrono::high_resolution_clock::now();

    render_image(framebuffer, camera, bvh_tree, lights, background_color, width, height, samples_per_pixel, max_depth, TraceType);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Render Time: " << elapsed.count() << " seconds\n";

    char outfile[] = "rendered_image.ppm";
    std::ofstream out(outfile);
    out << "P3\n"
        << width << ' ' << height << "\n255\n";

    for (const Color &color : framebuffer)
    {
        write_color(out, color, samples_per_pixel, j["camera"]["exposure"]);
    }
    out.close();

    std::cout << "Rendering complete. Image saved to " << outfile << std::endl;
    return 0;
}