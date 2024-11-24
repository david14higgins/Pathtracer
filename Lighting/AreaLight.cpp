#include "AreaLight.h"
#include <random>

// Constructor
AreaLight::AreaLight(const std::array<float, 3>& position, 
                     const std::array<float, 3>& intensity,
                     const std::array<float, 3>& u_axis,
                     const std::array<float, 3>& v_axis)
    : position(position), intensity(intensity),
      u_axis(Vector3::fromArray(u_axis)),
      v_axis(Vector3::fromArray(v_axis)) {}

// Get a random point on the area light
Vector3 AreaLight::getRandomPoint() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-0.5f, 0.5f);

    float u = dis(gen);
    float v = dis(gen);

    Vector3 basePos = Vector3::fromArray(position);
    return basePos + u_axis * u + v_axis * v;
}

// Get multiple sample points on the area light
std::vector<Vector3> AreaLight::getSamplePoints(int samples) const {
    std::vector<Vector3> points;
    points.reserve(samples);
    
    for (int i = 0; i < samples; ++i) {
        points.push_back(getRandomPoint());
    }
    
    return points;
}

const std::array<float, 3>& AreaLight::getPosition() const {
    return position;
}

const std::array<float, 3>& AreaLight::getIntensity() const {
    return intensity;
}

std::string AreaLight::toString() const {
    return "AreaLight";
}