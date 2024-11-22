#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include "../Materials/Material.h"
#include "Vector3.h" // Assuming you have a Vector3 class
#include "../Core/Ray.h"
#include <string>
#include "AABB.h"


class Shape {
public:
    // Add a constructor that initializes material
    Shape(const Material& material) : material(material) {}

    virtual ~Shape() = default;

    // Pure virtual function for intersection
    virtual bool intersect(const Ray& ray, float& t) const = 0;

    // Pure virtual function to compute the normal at a given point
    virtual Vector3 getNormal(const Vector3& point) const = 0;

    void setMaterial(const Material& m) { material = m; }
    const Material& getMaterial() const { return material; }

    virtual std::string toString() const;

    virtual void getUVCoordinates(const Vector3& point, float& u, float& v) const = 0;

    virtual AABB getBoundingBox() const = 0;


protected:
    Material material;  // This is the material for the shape
};

#endif