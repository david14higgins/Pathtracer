#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>
#include "Color.h"

class Texture {
public:
    Texture() : width(0), height(0) {} // Constructor
    bool loadFromPPM(const std::string& filename); // Load from PPM file
    Color getColorAt(float u, float v) const; // Get color at UV coordinates

private:
    std::vector<Color> pixels; // 1D vector of colors
    int width; // Width of the texture
    int height; // Height of the texture
};

#endif // TEXTURE_H