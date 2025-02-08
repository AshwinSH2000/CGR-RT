#pragma once
#include "Vector2.hpp"


class Texture {
public:
    int width, height;
    std::vector<Color> data;

    Texture(const std::string &filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return;
        }

        std::string header;
        file >> header;
        if (header != "P6") {
            std::cerr << "Invalid PPM file/Not a PPM file: " << filename << std::endl;
            return;
        }

        file >> width >> height;
        int max_val;
        file >> max_val;
        file.ignore(256, '\n'); 

        data.resize(width * height);
        for (int i = 0; i < width * height; ++i) {
            unsigned char rgb[3];
            file.read(reinterpret_cast<char*>(rgb), 3);
            data[i] = Color(rgb[0] / 255.0, rgb[1] / 255.0, rgb[2] / 255.0);
        }
    }

    Color sample(const Vector2 &uv) const {
        int x = static_cast<int>(uv.x * width) % width;
        int y = static_cast<int>(uv.y * height) % height;
        return data[y * width + x];
    }
};