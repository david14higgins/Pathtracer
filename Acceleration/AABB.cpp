#include "AABB.h"
#include <algorithm>
#include <iostream>

AABB::AABB() : min(Vector3()), max(Vector3()) {}

AABB::AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {}

bool AABB::intersect(const Ray& ray) const {
    Vector3 rayOrigin = ray.getOrigin();
    Vector3 rayDir = ray.getDirection();
    
    float tmin = -std::numeric_limits<float>::infinity();
    float tmax = std::numeric_limits<float>::infinity();
        
    // Test intersection for each axis
    for (int i = 0; i < 3; i++) {
        if (std::abs(rayDir[i]) < 1e-6) {
            if (rayOrigin[i] < min[i] || rayOrigin[i] > max[i]) {
                return false;
            }
            continue;
        }

        float invD = 1.0f / rayDir[i];
        float t0 = (min[i] - rayOrigin[i]) * invD;
        float t1 = (max[i] - rayOrigin[i]) * invD;
        
        if (invD < 0.0f) {
            std::swap(t0, t1);
        }
                
        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);
            
        if (tmax <= tmin + 1e-6) {
            return false;
        }
    }
    
    bool result = tmax >= 0.0f;
    return result;
}

AABB AABB::surrounding_box(const AABB& box1, const AABB& box2) {
    Vector3 small(
        std::min(box1.min[0], box2.min[0]),
        std::min(box1.min[1], box2.min[1]),
        std::min(box1.min[2], box2.min[2])
    );

    Vector3 big(
        std::max(box1.max[0], box2.max[0]),
        std::max(box1.max[1], box2.max[1]),
        std::max(box1.max[2], box2.max[2])
    );

    return AABB{small, big};
}