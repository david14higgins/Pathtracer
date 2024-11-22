#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include "Camera.h"
#include "Scene.h" 
#include "../Materials/Color.h"


class Renderer {
public:
    enum class RenderMode { BINARY, PHONG, PATHTRACER};

    // Constructor that takes a Scene object
    Renderer(int nbounces, RenderMode rendermode, const Camera& camera, const Scene& scene, bool useBVH, bool useAntiAliasing = false, int samplesPerPixel = 4);

    // Accessors
    int getNbounces() const;
    RenderMode getRenderMode() const;
    const Camera& getCamera() const;
    const Scene& getScene() const;  // New accessor for Scene

    // Method to render the scene
    std::vector<std::vector<Color>> renderScene();

    // Method to render a single pixel with default currentBounce
    Color renderPixel(const Ray& ray, int currentBounce = 0);

    // Method for tone mapping 
    Color toneMap(const Color& hdrColor) const; 

    // Method to return scene details as a string
    std::string toString() const;
    
private:
    int nbounces;
    RenderMode rendermode;
    Camera camera;
    Scene scene;  // Scene object containing all scene data
    bool useBVH;
    bool useAntiAliasing; 
    int samplesPerPixel; 

    Color tracePath(const Ray& ray, int depth = 0);
    Vector3 randomHemisphereDirection(const Vector3& normal);
    float randomFloat();
};

#endif // RENDERER_H