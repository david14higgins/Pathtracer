#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Shape.h"
#include "Vector3.h"
#include <sstream>

class Triangle : public Shape {
public:
    // Constructor declaration (only declare it here)
    Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Material& material);

    bool intersect(const Ray& ray, float& t) const override;

    Vector3 getNormal(const Vector3& point) const override;

    std::string toString() const override;

    virtual void getUVCoordinates(const Vector3& point, float& u, float& v) const override;

    AABB getBoundingBox() const override;


private:
    Vector3 v0, v1, v2;
};

#endif  // TRIANGLE_H