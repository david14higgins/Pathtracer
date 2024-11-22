#ifndef SCENE_H
#define SCENE_H

#include <array>
#include <vector>
#include <memory>
#include <string>
#include "../Lighting/Light.h"  // Assuming you have a base class for light types
#include "../Lighting/PointLight.h"  // Include the PointLight class
#include "../Geometry/Shape.h"  // Assuming you have a Shape base class for all shape types (e.g., Sphere, Triangle, Cylinder)
#include "../Geometry/BVHNode.h"

class Scene {
public:
    // Constructor to initialize the scene with a background color, light sources, and shapes
    Scene(const std::array<float, 3>& backgroundColor, 
          const std::vector<std::shared_ptr<Light>>& lightSources = {}, 
          const std::vector<std::shared_ptr<Shape>>& shapes = {});

    // Add light sources to the scene
    void addLight(std::shared_ptr<Light> light);

    // Add shapes to the scene
    void addShape(std::shared_ptr<Shape> shape);

    // Accessor methods
    const std::array<float, 3>& getBackgroundColor() const;
    const std::vector<std::shared_ptr<Light>>& getLightSources() const;
    const std::vector<std::shared_ptr<Shape>>& getShapes() const;

    // Method to return scene details as a string
    std::string toString() const;

    std::shared_ptr<BVHNode> getBVH();

private:
    std::array<float, 3> backgroundColor;  // Background color (RGB)
    std::vector<std::shared_ptr<Light>> lightSources;  // List of light sources
    std::vector<std::shared_ptr<Shape>> shapes;  // List of shapes in the scene
    std::shared_ptr<BVHNode> bvh;  // Add this line
};

#endif // SCENE_H