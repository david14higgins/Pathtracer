#ifndef SCENE_H
#define SCENE_H

#include <array>
#include <vector>
#include <memory>
#include <string>
#include "../Lighting/Light.h" 
#include "../Lighting/PointLight.h"  
#include "../Geometry/Shape.h" 
#include "../Acceleration/BVHNode.h"

class Scene {
public:
    // Constructor 
    Scene(const std::array<float, 3>& backgroundColor, 
          const std::vector<std::shared_ptr<Light>>& lightSources = {}, 
          const std::vector<std::shared_ptr<Shape>>& shapes = {});

    // Add light sources to the scene
    void addLight(std::shared_ptr<Light> light);

    // Add shapes to the scene
    void addShape(std::shared_ptr<Shape> shape);

    const std::array<float, 3>& getBackgroundColor() const;
    const std::vector<std::shared_ptr<Light>>& getLightSources() const;
    const std::vector<std::shared_ptr<Shape>>& getShapes() const;

    std::shared_ptr<BVHNode> getBVH();

    std::string toString() const;
private:
    std::array<float, 3> backgroundColor;  // Background color (RGB)
    std::vector<std::shared_ptr<Light>> lightSources;  // List of light sources
    std::vector<std::shared_ptr<Shape>> shapes;  // List of shapes in the scene
    std::shared_ptr<BVHNode> bvh;  // Add this line
};

#endif // SCENE_H