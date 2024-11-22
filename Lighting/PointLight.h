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

    // Override the toString() method to output the details of the PointLight
    std::string toString() const override;

private:
    std::array<float, 3> position;
    std::array<float, 3> intensity;
};

#endif // POINTLIGHT_H