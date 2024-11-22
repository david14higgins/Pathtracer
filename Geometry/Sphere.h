#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"
#include "Vector3.h"
#include <string>
#include <sstream>


class Sphere : public Shape {
public:
    // Constructor declaration (don't define it here)
    Sphere(const Vector3& center, float radius, const Material& material);

    bool intersect(const Ray& ray, float& t) const override;

    Vector3 getNormal(const Vector3& point) const override;
    
    std::string toString() const override;

    virtual void getUVCoordinates(const Vector3& point, float& u, float& v) const override;

    AABB getBoundingBox() const override;

private:
    Vector3 center;
    float radius;
};

#endif  // SPHERE_H
