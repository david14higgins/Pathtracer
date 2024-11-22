#include "Scene.h"
#include <iostream>
#include <sstream>

// Constructor to initialize the scene with a background color, light sources, and shapes
Scene::Scene(const std::array<float, 3>& backgroundColor, 
             const std::vector<std::shared_ptr<Light>>& lightSources, 
             const std::vector<std::shared_ptr<Shape>>& shapes)
    : backgroundColor(backgroundColor), lightSources(lightSources), shapes(shapes) {}

// Add a light to the scene
void Scene::addLight(std::shared_ptr<Light> light) {
    lightSources.push_back(light);
}

// Add a shape to the scene
void Scene::addShape(std::shared_ptr<Shape> shape) {
    shapes.push_back(shape);
}

// Accessor methods for background color, light sources, and shapes
const std::array<float, 3>& Scene::getBackgroundColor() const {
    return backgroundColor;
}

const std::vector<std::shared_ptr<Light>>& Scene::getLightSources() const {
    return lightSources;
}

const std::vector<std::shared_ptr<Shape>>& Scene::getShapes() const {
    return shapes;
}

std::shared_ptr<BVHNode> Scene::getBVH() {
    if (!bvh && !shapes.empty()) {
        bvh = std::make_shared<BVHNode>(shapes, 0, shapes.size());
    }
    return bvh;
}

std::string Scene::toString() const {
    std::ostringstream oss;
    oss << "Background Color: ["
        << backgroundColor[0] << ", "
        << backgroundColor[1] << ", "
        << backgroundColor[2] << "]\n";
    
    oss << "Light Sources (" << lightSources.size() << "):\n";
    for (const auto& light : lightSources) {
        oss << "  " << light->toString() << "\n";
    }
    
    oss << "Shapes (" << shapes.size() << "):\n";
    for (const auto& shape : shapes) {
        oss << "  " << shape->toString() << "\n";
    }
    
    return oss.str();
}