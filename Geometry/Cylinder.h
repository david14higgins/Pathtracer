#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include "Vector3.h"
#include <sstream>


class Cylinder : public Shape {
public:
    // Constructor declaration
    Cylinder(const Vector3& center, const Vector3& axis, float radius, float height, const Material& material);

    bool intersect(const Ray& ray, float& t) const override;

    Vector3 getNormal(const Vector3& point) const override;

    std::string toString() const override;

    void getUVCoordinates(const Vector3& point, float& u, float& v) const override;

    AABB getBoundingBox() const override;

private:
    Vector3 center;
    Vector3 axis;
    float radius;
    float height;
};

#endif