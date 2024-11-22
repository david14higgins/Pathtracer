#ifndef CAMERA_H
#define CAMERA_H

#include <array>
#include <string>
#include "Ray.h"

class Camera {
public:
    enum class CameraType { PINHOLE, THIN_LENS };

    // Constructor with JSON parameters
    Camera(CameraType type, int width, int height, 
           std::array<float, 3> position, std::array<float, 3> lookAt, 
           std::array<float, 3> upVector, float fov, float exposure, float aperture, float focalDistance);

    // Accessors
    const std::array<float, 3>& getPosition() const;
    const std::array<float, 3>& getLookAt() const;
    const std::array<float, 3>& getUpVector() const;
    float getFOV() const;
    float getExposure() const;
    int getWidth() const;
    int getHeight() const;
    float getAperture() const;
    float getFocalDistance() const;

    // Method to return camera details as a string
    std::string toString() const;

    // Method to generate a ray for a given pixel (x, y)
    Ray generateRay(int x, int y) const;  // Declaration of the generateRay method
    Ray generateRay(float x, float y) const;
private:
    CameraType type;
    int width, height;
    std::array<float, 3> position;
    std::array<float, 3> lookAt;
    std::array<float, 3> upVector;
    float fov;
    float exposure;
    float aperture;
    float focalDistance;
};

#endif // CAMERA_H