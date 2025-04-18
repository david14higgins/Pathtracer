#include "Camera.h"
#include <sstream>
#include <cmath>
#include "../Geometry/Vector3.h"

#define M_PI 3.14159265358979323846 

// Constructor
Camera::Camera(CameraType type, int width, int height, 
               std::array<float, 3> position, std::array<float, 3> lookAt, 
               std::array<float, 3> upVector, float fov, float exposure, float aperture, float focalDistance)
    : type(type), width(width), height(height), 
      position(position), lookAt(lookAt), upVector(upVector), 
      fov(fov), exposure(exposure), aperture(aperture), focalDistance(focalDistance) 
{}

const std::array<float, 3>& Camera::getPosition() const {
    return position;
}

const std::array<float, 3>& Camera::getLookAt() const {
    return lookAt;
}

const std::array<float, 3>& Camera::getUpVector() const {
    return upVector;
}

float Camera::getFOV() const {
    return fov;
}

float Camera::getExposure() const {
    return exposure;
}

int Camera::getWidth() const {
    return width;
}

int Camera::getHeight() const {
    return height;
}

float Camera::getAperture() const {
    return aperture;
}

float Camera::getFocalDistance() const {
    return focalDistance;
}

// Generate ray given pixels
Ray Camera::generateRay(int x, int y) const {
    return generateRay(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);
}

// Generate ray given float coordinates
Ray Camera::generateRay(float x, float y) const {
    // Calculate aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    // Calculate field of view scale
    float fovScale = std::tan(fov * 0.5f * M_PI / 180.0f);

    // Calculate pixel coordinates
    float px = -(2.0f * x / width - 1.0f) * aspectRatio * fovScale;
    float py = (1.0f - 2.0f * y / height) * fovScale;

    // Calculate coordinate system
    Vector3 forward, right, up;
    calculateCoordinateSystem(forward, right, up);

    if (type == CameraType::PINHOLE) { // Generate pinhole ray
        Vector3 origin = Vector3::fromArray(position);
        Vector3 direction = (forward + right * px + up * py).normalize();

        return Ray(origin, direction);
    } else if (type == CameraType::THIN_LENS) { // Generate thin lens ray
        // Calculate point on focal plane
        Vector3 focalPoint = Vector3::fromArray(position) + (forward + right * px + up * py).normalize() * focalDistance;
        // Generate random point on lens
        Vector3 lensPoint = sampleLensPoint();
        
        // Create ray from lens point to focal point
        return Ray(lensPoint, (focalPoint - lensPoint).normalize());
    } else {
        throw std::invalid_argument("Invalid camera type");
    }
}

// Calculate coordinate system
void Camera::calculateCoordinateSystem(Vector3& forward, Vector3& right, Vector3& up) const {
    forward = (Vector3::fromArray(lookAt) - Vector3::fromArray(position)).normalize();
    right = forward.cross(Vector3::fromArray(upVector)).normalize();
    up = right.cross(forward).normalize();
}
 
// Sample lens point 
Vector3 Camera::sampleLensPoint() const {
    // Generate random point on circular lens
    float r = std::sqrt(static_cast<float>(rand()) / RAND_MAX) * aperture;
    float theta = 2.0f * M_PI * static_cast<float>(rand()) / RAND_MAX;
    
    // Calculate coordinate system
    Vector3 forward, right, up;
    calculateCoordinateSystem(forward, right, up);
    
    return Vector3::fromArray(position) + (right * std::cos(theta) + up * std::sin(theta)) * r;
}

std::string Camera::toString() const {
    std::ostringstream oss;
    oss << "Camera Type: " << (type == CameraType::PINHOLE ? "Pinhole" : "Thin Lens") << "\n"
        << "Resolution: " << width << "x" << height << "\n"
        << "Position: [" << position[0] << ", " << position[1] << ", " << position[2] << "]\n"
        << "Look At: [" << lookAt[0] << ", " << lookAt[1] << ", " << lookAt[2] << "]\n"
        << "Up Vector: [" << upVector[0] << ", " << upVector[1] << ", " << upVector[2] << "]\n"
        << "FOV: " << fov << " degrees\n"
        << "Exposure: " << exposure << "\n"
        << "Aperture: " << aperture << "\n"
        << "Focal Distance: " << focalDistance;
    return oss.str();
}
