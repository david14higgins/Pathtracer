#ifndef AABB_H
#define AABB_H

#include "../Geometry/Vector3.h"
#include "../Core/Ray.h"

class AABB {
public:
    Vector3 min;
    Vector3 max;

    AABB();
    AABB(const Vector3& min, const Vector3& max);
    bool intersect(const Ray& ray) const;
    static AABB surrounding_box(const AABB& box1, const AABB& box2);
};

#endif