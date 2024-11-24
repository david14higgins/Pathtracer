#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include "Vector3.h"
#include <sstream>


class Cylinder : public Shape {
public:
    Cylinder(const Vector3& center, const Vector3& axis, float radius, float height, const Material& material); // Constructor

    bool intersect(const Ray& ray, float& t) const override; // Intersection test with a ray

    Vector3 getNormal(const Vector3& point) const override; // Get the normal at a point

    std::string toString() const override;

    void getUVCoordinates(const Vector3& point, float& u, float& v) const override; // Get the UV coordinates at a point

    AABB getBoundingBox() const override; // Get the bounding box of the cylinder

private:
    Vector3 center; // Center of the cylinder
    Vector3 axis; // Axis of the cylinder
    float radius; // Radius of the cylinder
    float height; // Height of the cylinder
};

#endif 