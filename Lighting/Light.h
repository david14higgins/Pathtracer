#ifndef LIGHT_H
#define LIGHT_H

#include <array>
#include <string>
#include <memory>

class Light {
public:
    virtual ~Light() = default;

    // Pure virtual function to get the type of the light source
    virtual std::string toString() const = 0;

    // Pure virtual functions for position and intensity
    virtual const std::array<float, 3>& getPosition() const = 0;
    virtual const std::array<float, 3>& getIntensity() const = 0;
};

#endif // LIGHT_H