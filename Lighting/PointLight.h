#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"
#include <array>
#include <string>

class PointLight : public Light {
public:
    // Constructor for PointLight
    PointLight(const std::array<float, 3>& position, const std::array<float, 3>& intensity);

    // Accessor methods
    const std::array<float, 3>& getPosition() const override;
    const std::array<float, 3>& getIntensity() const override;

    std::string toString() const override;

private:
    std::array<float, 3> position; // Position of the point light
    std::array<float, 3> intensity; // Intensity of the point light
};

#endif // POINTLIGHT_H