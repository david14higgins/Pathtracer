#ifndef CAMERA_H
#define CAMERA_H

#include <array>
#include <string>
#include "Ray.h"

class Camera {
public:
    // Enum for camera types
    enum class CameraType { PINHOLE, THIN_LENS };

    // Constructor 
    Camera(CameraType type, int width, int height, 
           std::array<float, 3> position, std::array<float, 3> lookAt, 
           std::array<float, 3> upVector, float fov, float exposure, float aperture, float focalDistance);

    const std::array<float, 3>& getPosition() const;
    const std::array<float, 3>& getLookAt() const;
    const std::array<float, 3>& getUpVector() const;
    float getFOV() const;
    float getExposure() const;
    int getWidth() const;
    int getHeight() const;
    float getAperture() const;
    float getFocalDistance() const;

    std::string toString() const;

    // Method to generate a ray for a given pixel (x, y) takes integer and float coordinates
    Ray generateRay(int x, int y) const; 
    Ray generateRay(float x, float y) const;
private:
    CameraType type; // Camera type
    int width, height; // Width and height of the camera
    std::array<float, 3> position; // Position of the camera
    std::array<float, 3> lookAt; // Look at point of the camera
    std::array<float, 3> upVector; // Up vector of the camera
    float fov; // Field of view of the camera
    float exposure; // Exposure of the camera
    float aperture; // Aperture of the camera
    float focalDistance; // Focal distance of the camera

    // Helper method for calculating the camera's coordinate system
    void calculateCoordinateSystem(Vector3& forward, Vector3& right, Vector3& up) const;

    // Helper method for generating random points on the lens
    Vector3 sampleLensPoint() const;
};

#endif // CAMERA_H