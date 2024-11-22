#include "Texture.h"
#include <fstream>
#include <sstream>
#include <iostream>

bool Texture::loadFromPPM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);  // Open in binary mode
    if (!file.is_open()) {
        std::cerr << "Failed to open texture file: " << filename << std::endl;
        return false;
    }

    std::string magic;
    file >> magic;
    file.get();  // Skip newline

    // Skip comments
    while (file.peek() == '#') {
        std::string comment;
        std::getline(file, comment);
    }

    file >> width >> height;
    file.get();  // Skip newline
    
    int maxVal;
    file >> maxVal;
    file.get();  // Skip newline after maxVal

    pixels.resize(width * height);

    if (magic == "P3") {  // ASCII format
        for (int i = 0; i < width * height; ++i) {
            int r, g, b;
            file >> r >> g >> b;
            pixels[i] = Color(r, g, b);
        }
    }
    else if (magic == "P6") {  // Binary format
        std::vector<unsigned char> buffer(width * height * 3);
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

        for (int i = 0; i < width * height; ++i) {
            unsigned char r = buffer[i * 3];
            unsigned char g = buffer[i * 3 + 1];
            unsigned char b = buffer[i * 3 + 2];
            pixels[i] = Color(r, g, b);
        }
    }
    else {
        std::cerr << "Unsupported PPM format: " << magic << std::endl;
        return false;
    }

    return true;
}

Color Texture::getColorAt(float u, float v) const {
    // Ensure u and v are in [0,1]
    u = u - floor(u);
    v = v - floor(v);

    // Convert to pixel coordinates
    int x = static_cast<int>(u * (width - 1));
    int y = static_cast<int>(v * (height - 1));

    // Get pixel from texture
    return pixels[y * width + x];
}