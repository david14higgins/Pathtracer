#include "Texture.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Load texture from PPM file
bool Texture::loadFromPPM(const std::string& filename) { 
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open texture file: " << filename << std::endl;
        return false;
    }

    // Read the magic number
    std::string magic;
    file >> magic;
    file.get();  // Skip newline

    // Skip comments
    while (file.peek() == '#') {
        std::string comment;
        std::getline(file, comment);
    }

    // Read the width and height
    file >> width >> height;
    file.get();  // Skip newline
    
    // Read the maximum value
    int maxVal;
    file >> maxVal;
    file.get();  // Skip newline after maxVal

    // Resize the pixels vector to the width and height of the texture
    pixels.resize(width * height);

    // Read the pixels in ASCII or binary format
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

    // Close the file
    file.close();
    return true;
}

// Get color at UV coordinates
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