#include "AABB.h"
#include <algorithm>
#include <iostream>

AABB::AABB() : min(Vector3()), max(Vector3()) {} // Default constructor

AABB::AABB(const Vector3& min, const Vector3& max) : min(min), max(max) {} // Constructor with min and max points

// Intersection test with a ray
bool AABB::intersect(const Ray& ray) const {
    Vector3 rayOrigin = ray.getOrigin();
    Vector3 rayDir = ray.getDirection();
    
    // Initialize tmin and tmax to negative and positive infinity respectively
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

        // Calculate inverse of ray direction
        float invD = 1.0f / rayDir[i];
        float t0 = (min[i] - rayOrigin[i]) * invD;
        float t1 = (max[i] - rayOrigin[i]) * invD;
        
        // Swap t0 and t1 if ray direction is negative
        if (invD < 0.0f) {
            std::swap(t0, t1);
        }
                
        // Update tmin and tmax
        tmin = std::max(tmin, t0);
        tmax = std::min(tmax, t1);
            
        // Check if ray misses the AABB
        if (tmax <= tmin + 1e-6) {
            return false;
        }
    }
    
    // Check if ray intersects the AABB
    bool result = tmax >= 0.0f;
    return result;
}

// Create a surrounding AABB for two boxes
AABB AABB::surrounding_box(const AABB& box1, const AABB& box2) {
    // Calculate the minimum and maximum points of the surrounding AABB
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