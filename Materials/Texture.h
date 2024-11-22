#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>
#include "Color.h"

class Texture {
public:
    Texture() : width(0), height(0) {}
    bool loadFromPPM(const std::string& filename);
    Color getColorAt(float u, float v) const;

private:
    std::vector<Color> pixels;
    int width;
    int height;
};

#endif // TEXTURE_H