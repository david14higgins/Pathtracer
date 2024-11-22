#include "Camera.h"
#include <sstream>
#include "../Geometry/Vector3.h"
#include <cmath>

#define M_PI 3.14159265358979323846 // If M_PI is not already defined

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
// Non-antialiased raytracer passes pixels as integers
Ray Camera::generateRay(int x, int y) const {
    return generateRay(static_cast<float>(x) + 0.5f, static_cast<float>(y) + 0.5f);
}

Ray Camera::generateRay(float x, float y) const {
    // Compute normalized device coordinates for the pixel (x, y)
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
    float fovScale = std::tan(fov * 0.5f * M_PI / 180.0f);

    // Calculate the pixel's position in NDC space (-1 to 1 range)
    // Note: Removed the +0.5f offset since it's now handled by the caller
    float px = -(2.0f * x / width - 1.0f) * aspectRatio * fovScale;
    float py = (1.0f - 2.0f * y / height) * fovScale;

    // Generate the direction of the ray
    Vector3 forward = (Vector3::fromArray(lookAt) - Vector3::fromArray(position)).normalize();
    Vector3 right = forward.cross(Vector3::fromArray(upVector)).normalize();
    Vector3 up = right.cross(forward).normalize();

    // Calculate the ray direction
    Vector3 rayDirection = forward + right * px + up * py;
    rayDirection = rayDirection.normalize();

    return Ray(Vector3::fromArray(position), rayDirection);
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
