#ifndef AREALIGHT_H
#define AREALIGHT_H

#include "Light.h"
#include "../Geometry/Vector3.h"
#include <array>
#include <string>
#include <vector>

class AreaLight : public Light {
public:
    // Constructor 
    AreaLight(const std::array<float, 3>& position, 
             const std::array<float, 3>& intensity,
             const std::array<float, 3>& u_axis,
             const std::array<float, 3>& v_axis);

    // Get a random point on the area light
    Vector3 getRandomPoint() const;
    
    // Get multiple sample points on the area light
    std::vector<Vector3> getSamplePoints(int samples) const;

    const std::array<float, 3>& getPosition() const override;
    const std::array<float, 3>& getIntensity() const override;
    const Vector3& getUAxis() const { return u_axis; }
    const Vector3& getVAxis() const { return v_axis; }

    std::string toString() const override;

private:
    std::array<float, 3> position;  // Center position of the area light
    std::array<float, 3> intensity; // Intensity of the area light
    Vector3 u_axis; // First axis defining the light's area
    Vector3 v_axis; // Second axis defining the light's area
};

#endif // AREALIGHT_H