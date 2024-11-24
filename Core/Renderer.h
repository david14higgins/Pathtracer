#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include "Camera.h"
#include "Scene.h" 
#include "../Materials/Color.h"


class Renderer {
public:
    // Enum for render modes
    enum class RenderMode { BINARY, PHONG, PATHTRACER};

    // Constructor
    Renderer(int nbounces, RenderMode rendermode, const Camera& camera, const Scene& scene, bool useBVH, bool useAntiAliasing = false, int samplesPerPixel = 4);

    int getNbounces() const;
    RenderMode getRenderMode() const;
    const Camera& getCamera() const;
    const Scene& getScene() const; 

    // Method to render the scene
    std::vector<std::vector<Color>> renderScene();

    std::string toString() const;
    
private:
    int nbounces; // Number of bounces
    RenderMode rendermode; // Render mode
    Camera camera; // Camera object
    Scene scene; // Scene object
    bool useBVH; // Use BVH flag 
    bool useAntiAliasing; // Use anti-aliasing flag
    int samplesPerPixel; // Samples per pixel in anti-aliasing

    // Method to render a single pixel with default currentBounce
    Color renderPixel(const Ray& ray, int currentBounce = 0);

    // Methods for different render modes
    Color renderBinary(const Ray& ray);

    Color renderBlinnPhong(const Ray& ray, int currentBounce);

    Color renderPathTracer(const Ray& ray);
    
    // Method to trace a path for a single pixel in path tracer
    Color tracePath(const Ray& ray, int depth = 0);

    // Method to generate a random hemisphere direction in path tracer
    Vector3 randomHemisphereDirection(const Vector3& normal);

    // Method to generate a random float in path tracer
    float randomFloat();

    // Helper methods
    bool findClosestIntersection(const Ray& ray, float& minDistance, 
                                std::shared_ptr<Shape>& closestShape,
                                Vector3& intersectionPoint, Vector3& normal);
    bool isInShadow(const Ray& shadowRay, float lightDistance);

    // Method for tone mapping 
    Color toneMap(const Color& hdrColor) const; 
    
   
};

#endif // RENDERER_H