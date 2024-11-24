#ifndef SPHERE_H
#define SPHERE_H

#include <string>
#include <sstream>
#include "Shape.h"
#include "Vector3.h"

class Sphere : public Shape {
public:
    Sphere(const Vector3& center, float radius, const Material& material); // Constructor

    bool intersect(const Ray& ray, float& t) const override; // Intersection test with a ray

    Vector3 getNormal(const Vector3& point) const override; // Get the normal at a point
    
    std::string toString() const override;

    virtual void getUVCoordinates(const Vector3& point, float& u, float& v) const override; // Get the UV coordinates at a point

    AABB getBoundingBox() const override; // Get the bounding box of the sphere

private:
    Vector3 center; // Center of the sphere
    float radius; // Radius of the sphere
};

#endif  // SPHERE_H
