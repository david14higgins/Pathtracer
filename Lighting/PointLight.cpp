#include "PointLight.h"
#include <sstream>

// Constructor for PointLight
PointLight::PointLight(const std::array<float, 3>& position, const std::array<float, 3>& intensity)
    : position(position), intensity(intensity) {}

const std::array<float, 3>& PointLight::getPosition() const{
    return position;
}

const std::array<float, 3>& PointLight::getIntensity() const{
    return intensity;
}

std::string PointLight::toString() const {
    std::ostringstream oss;
    oss << "PointLight (Position: [" 
        << position[0] << ", " << position[1] << ", " << position[2] << "], "
        << "Intensity: [" << intensity[0] << ", " << intensity[1] << ", " << intensity[2] << "])";
    return oss.str();
}