#ifndef RAY_H
#define RAY_H

#include "../Geometry/Vector3.h"

class Ray {
public:
    // Constructor that takes the origin and direction of the ray
    Ray(const Vector3& origin, const Vector3& direction)
        : origin(origin), direction(direction.normalize()) {}

    // Accessor methods
    const Vector3& getOrigin() const { return origin; }
    const Vector3& getDirection() const { return direction; }

    // Method to get a point along the ray at a distance t
    Vector3 at(float t) const {
        return origin + direction * t;
    }

    // Add toString method declaration
    std::string toString() const;

private:
    Vector3 origin;   // Origin of the ray
    Vector3 direction; // Direction of the ray
};

#endif // RAY_H