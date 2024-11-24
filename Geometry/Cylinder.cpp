#include "Cylinder.h"
#include <cmath>
#include <algorithm>
#define M_PI 3.14159265358979323846

// Constructor for the cylinder
Cylinder::Cylinder(const Vector3& center, const Vector3& axis, float radius, float height, const Material& material)
    : Shape(material),center(center), axis(axis.normalize()), radius(radius), height(height) {}

// Intersection test with a ray
bool Cylinder::intersect(const Ray& ray, float& t) const {
    // Calculate the vector from the ray origin to the cylinder center
    Vector3 oc = ray.getOrigin() - center;
    // Calculate the projection of the ray direction onto the plane perpendicular to the cylinder axis
    Vector3 d_proj = ray.getDirection() - axis * ray.getDirection().dot(axis);
    // Calculate the projection of the vector from the ray origin to the cylinder center onto the plane perpendicular to the cylinder axis
    Vector3 oc_proj = oc - axis * oc.dot(axis);

    // Calculate the coefficients of the quadratic equation
    float a = d_proj.dot(d_proj);
    float b = 2.0f * d_proj.dot(oc_proj);
    float c = oc_proj.dot(oc_proj) - radius * radius;

    // Calculate the discriminant of the quadratic equation
    float discriminant = b * b - 4 * a * c;
    float tCylinder = std::numeric_limits<float>::infinity();
    if (discriminant >= 0) {
        float sqrtDiscriminant = std::sqrt(discriminant);
        float t0 = (-b - sqrtDiscriminant) / (2.0f * a);
        float t1 = (-b + sqrtDiscriminant) / (2.0f * a);

        if (t0 > 0) {
            tCylinder = t0;
        } else if (t1 > 0) {
            tCylinder = t1;
        }

        // Calculate the intersection point
        Vector3 point = ray.getOrigin() + ray.getDirection() * tCylinder;
        float heightAtIntersection = (point - center).dot(axis);

        // Check if the intersection point is outside the cylinder
        if (heightAtIntersection < -height || heightAtIntersection > height) {
            tCylinder = std::numeric_limits<float>::infinity();
        }
    }

    // Calculate the centers of the top and bottom caps
    Vector3 topCenter = center + axis * height;
    Vector3 bottomCenter = center - axis * height;

    // Calculate the intersection point with the top cap
    float tTopCap = (axis.dot(topCenter - ray.getOrigin())) / axis.dot(ray.getDirection());
    Vector3 pointTopCap = ray.getOrigin() + ray.getDirection() * tTopCap;
    bool hitTopCap = (pointTopCap - topCenter).length() <= radius && tTopCap > 0;

    // Calculate the intersection point with the bottom cap
    float tBottomCap = (axis.dot(bottomCenter - ray.getOrigin())) / axis.dot(ray.getDirection());
    Vector3 pointBottomCap = ray.getOrigin() + ray.getDirection() * tBottomCap;
    bool hitBottomCap = (pointBottomCap - bottomCenter).length() <= radius && tBottomCap > 0;

    // Calculate the intersection point with the cylinder
    float tCap = std::min(hitTopCap ? tTopCap : std::numeric_limits<float>::infinity(),
                          hitBottomCap ? tBottomCap : std::numeric_limits<float>::infinity());

    // Select the closest intersection point
    t = std::min(tCylinder, tCap);

    // Return true if there is an intersection
    return t != std::numeric_limits<float>::infinity();
}

// Get the normal at a point
Vector3 Cylinder::getNormal(const Vector3& point) const {
    // Calculate the height at the point
    float heightAtPoint = (point - center).dot(axis);
    if (std::fabs(heightAtPoint - height) < 1e-6) {
        return axis; // Top cap normal
    } else if (std::fabs(heightAtPoint + height) < 1e-6) {
        return axis * -1; // Bottom cap normal
    } else {
        // Calculate the vector from the cylinder center to the point
        Vector3 cp = point - center;
        // Calculate the projection length of the vector onto the cylinder axis
        float projectionLength = cp.dot(axis);
        // Calculate the component of the vector along the cylinder axis
        Vector3 axisComponent = axis * projectionLength;
        // Return the normal at the point
        return (cp - axisComponent).normalize();
    }
}

std::string Cylinder::toString() const {
    std::ostringstream oss;
    oss << "Cylinder {" << std::endl
        << "  Center: " << center.toString() << std::endl
        << "  Axis: " << axis.toString() << std::endl
        << "  Radius: " << radius << std::endl
        << "  Height: " << height << std::endl
        << "  Material: " << getMaterial().toString() << std::endl
        << "}";
    return oss.str();
}

// Get the UV coordinates at a point
void Cylinder::getUVCoordinates(const Vector3& point, float& u, float& v) const { 
    // Calculate the vector from the cylinder center to the point
    Vector3 dir = point - center;
    
    // Calculate angle around cylinder
    u = 0.5f + atan2(dir.z, dir.x) / (2.0f * M_PI);
    
    // Calculate height position
    float heightPos = dir.dot(axis);
    v = (heightPos + height) / (2.0f * height);
}

// Get the bounding box of the cylinder
AABB Cylinder::getBoundingBox() const {
    // Calculate the endpoints of the cylinder's central axis
    Vector3 halfAxis = axis * (height / 2.0f);
    Vector3 endpoint1 = center + halfAxis;
    Vector3 endpoint2 = center - halfAxis;
    
    // Create a box that encompasses both end caps
    Vector3 min(
        std::min(endpoint1.x, endpoint2.x) - radius,
        std::min(endpoint1.y, endpoint2.y) - radius,
        std::min(endpoint1.z, endpoint2.z) - radius
    );
    
    Vector3 max(
        std::max(endpoint1.x, endpoint2.x) + radius,
        std::max(endpoint1.y, endpoint2.y) + radius,
        std::max(endpoint1.z, endpoint2.z) + radius
    );
    
    return AABB(min, max);
}