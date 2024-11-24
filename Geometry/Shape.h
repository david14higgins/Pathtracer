#ifndef SHAPE_H
#define SHAPE_H

#include <memory>
#include <string>
#include "Vector3.h" 
#include "../Materials/Material.h"
#include "../Core/Ray.h"
#include "../Acceleration/AABB.h"


class Shape {
public:
    
    Shape(const Material& material) : material(material) {} // Constructor that initializes material

    virtual ~Shape() = default; // Destructor

    virtual bool intersect(const Ray& ray, float& t) const = 0; // Pure virtual function to intersect with a ray

    virtual Vector3 getNormal(const Vector3& point) const = 0; // Pure virtual function to get the normal at a point

    void setMaterial(const Material& m) { material = m; } 
    const Material& getMaterial() const { return material; }

    virtual std::string toString() const;

    virtual void getUVCoordinates(const Vector3& point, float& u, float& v) const = 0; // Pure virtual function to get the UV coordinates at a point

    virtual AABB getBoundingBox() const = 0; // Pure virtual function to get the bounding box of the shape


protected:
    Material material;  // This is the material for the shape
};

#endif