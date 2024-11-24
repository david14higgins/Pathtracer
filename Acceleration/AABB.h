#ifndef AABB_H
#define AABB_H

#include "../Geometry/Vector3.h"
#include "../Core/Ray.h"

class AABB {
public:
    Vector3 min; // Minimum point of the AABB
    Vector3 max; // Maximum point of the AABB

    AABB(); // Default constructor
    AABB(const Vector3& min, const Vector3& max); // Constructor with min and max points
    bool intersect(const Ray& ray) const; // Intersection test with a ray
    static AABB surrounding_box(const AABB& box1, const AABB& box2); // Create a surrounding AABB for two boxes
};

#endif