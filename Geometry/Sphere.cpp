#include "Sphere.h"
#include <cmath>
#define M_PI 3.14159265358979323846

// Constructor for the sphere
Sphere::Sphere(const Vector3& center, float radius, const Material& material)
    : Shape(material), center(center), radius(radius) {}

// Intersection test with a ray
bool Sphere::intersect(const Ray& ray, float& t) const {
    // Calculate vector from ray origin to sphere center
    Vector3 oc = ray.getOrigin() - center;
    
    // Calculate coefficients of the quadratic equation
    float a = ray.getDirection().dot(ray.getDirection());
    float b = 2.0f * oc.dot(ray.getDirection());
    float c = oc.dot(oc) - radius * radius;
    
    // Calculate the discriminant
    float discriminant = b * b - 4 * a * c;
    
    // Check if there is an intersection
    if (discriminant < 0) {
        return false;  // No intersection
    } else {
        // Calculate the nearest t value for intersection
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t1 = (-b + sqrtDiscriminant) / (2.0f * a);
        
        // We want the nearest positive t value
        if (t0 > 0) {
            t = t0;
        } else if (t1 > 0) {
            t = t1;
        } else {
            return false;  // Both t values are negative, intersection is behind the ray
        }
        return true;
    }
}

// Get the normal at a point
Vector3 Sphere::getNormal(const Vector3& point) const {
    // Calculate the vector from the center of the sphere to the point
    Vector3 normal = (point - center).normalize();
    return normal;
}


std::string Sphere::toString() const {
    std::stringstream ss;
    ss << "Sphere: Center(" << center.x << ", " << center.y << ", " << center.z << "), "
       << "Radius: " << radius << "\n";
    ss << "Material: " << material.toString();
    return ss.str();
}

// Get the UV coordinates at a point
void Sphere::getUVCoordinates(const Vector3& point, float& u, float& v) const {
    Vector3 dir = (point - center).normalize();
    
    // Convert to spherical coordinates
    u = 0.5f + atan2(dir.z, dir.x) / (2.0f * M_PI);
    v = 0.5f - asin(dir.y) / M_PI;
}

AABB Sphere::getBoundingBox() const {
    Vector3 min = center - Vector3(radius, radius, radius);
    Vector3 max = center + Vector3(radius, radius, radius);
    return AABB(min, max);
}