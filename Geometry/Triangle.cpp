#include "Triangle.h"
#include <cmath> 
#include <algorithm>
#include <initializer_list>

// Constructor for the triangle
Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Material& material)
    : Shape(material), v0(v0), v1(v1), v2(v2) {}

// Intersection test with a ray
bool Triangle::intersect(const Ray& ray, float& t) const {
    // Compute the two edges of the triangle
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    // Compute the determinant
    Vector3 h = ray.getDirection().cross(edge2);
    float a = edge1.dot(h);

    // If the determinant is near zero, the ray is parallel to the triangle
    if (fabs(a) < 1e-6) {
        return false;  // No intersection
    }

    // Compute the inverse of the determinant
    float f = 1.0f / a;

    // Compute the vector from the ray origin to the triangle vertex
    Vector3 s = ray.getOrigin() - v0;

    // Compute the u parameter
    float u = f * s.dot(h);
    if (u < 0.0f || u > 1.0f) {
        return false;  // Intersection is outside of the triangle
    }

    // Compute the v parameter
    Vector3 q = s.cross(edge1);
    float v = f * ray.getDirection().dot(q);
    if (v < 0.0f || u + v > 1.0f) {
        return false;  // Intersection is outside of the triangle
    }

    // Compute the t parameter (distance to the intersection point)
    t = f * edge2.dot(q);
    if (t > 1e-6) {
        return true;  // There is an intersection
    }

    return false;  // Intersection is behind the ray
}

// Get the normal at a point
Vector3 Triangle::getNormal(const Vector3& point) const {
    // Calculate two edges of the triangle
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    // Calculate the normal using the cross product
    Vector3 normal = edge1.cross(edge2).normalize();

    return normal;
}

std::string Triangle::toString() const {
    std::stringstream ss;
    ss << "Triangle: V0: " << v0 << ", v1:" << v1 << ", v2:" << v2 << "\n";
    ss << "Material: " << material.toString();
    return ss.str();
}

// Get the UV coordinates at a point
void Triangle::getUVCoordinates(const Vector3& point, float& u, float& v) const {
    // Simple planar mapping based on triangle vertices
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;
    Vector3 p = point - v0;
    
    // Calculate the determinants
    float d00 = edge1.dot(edge1);
    float d01 = edge1.dot(edge2);
    float d11 = edge2.dot(edge2);
    float d20 = p.dot(edge1);
    float d21 = p.dot(edge2);
    
    // Calculate the denominator
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    u = (d00 * d21 - d01 * d20) / denom;
}

// Get the bounding box of the triangle
AABB Triangle::getBoundingBox() const {
    Vector3 min_bounds(
        std::min(std::min(v0[0], v1[0]), v2[0]),
        std::min(std::min(v0[1], v1[1]), v2[1]),
        std::min(std::min(v0[2], v1[2]), v2[2])
    );
    
    Vector3 max_bounds(
        std::max(std::max(v0[0], v1[0]), v2[0]),
        std::max(std::max(v0[1], v1[1]), v2[1]),
        std::max(std::max(v0[2], v1[2]), v2[2])
    );

    // Add a small epsilon to avoid precision issues
    const float epsilon = 1e-4f;
    min_bounds = min_bounds - Vector3(epsilon, epsilon, epsilon);
    max_bounds = max_bounds + Vector3(epsilon, epsilon, epsilon);

    return AABB(min_bounds, max_bounds);
}