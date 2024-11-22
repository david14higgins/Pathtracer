#include "Renderer.h"
#include <sstream>
#include <limits> // for std::numeric_limits<float>::infinity()
#include <fstream>
#include <vector>
#include <algorithm>

#include "Ray.h"
#include "../Geometry/Vector3.h"
#include "../Geometry/Shape.h"
#include "../Materials/Material.h"
#include "../Materials/Color.h"
#include "../Lighting/Light.h"

#include <filesystem>
#include <system_error>



// Constructor that takes a Scene object
Renderer::Renderer(int nbounces, RenderMode rendermode, const Camera& camera, const Scene& scene, bool useBVH, bool useAntiAliasing, int samplesPerPixel)
    : nbounces(nbounces), rendermode(rendermode), camera(camera), scene(scene), useBVH(useBVH), useAntiAliasing(useAntiAliasing), samplesPerPixel(samplesPerPixel) {}

// Accessor methods
int Renderer::getNbounces() const {
    return nbounces;
}

Renderer::RenderMode Renderer::getRenderMode() const {
    return rendermode;
}

const Camera& Renderer::getCamera() const {
    return camera;
}

const Scene& Renderer::getScene() const {
    return scene;
}



// Method to render the scene and return a 2D array of pixel colors
std::vector<std::vector<Color>> Renderer::renderScene() {
    // Get dimensions from camera
    int width = camera.getWidth();
    int height = camera.getHeight();
    int totalPixels = width * height;
    int pixelsCompleted = 0;
    int lastPercentage = -10;

    // Create the pixel color buffer
    std::vector<std::vector<Color>> pixelColors(height, std::vector<Color>(width));


    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!useAntiAliasing) {
            Ray ray = camera.generateRay(x, y);
            pixelColors[y][x] = renderPixel(ray, 0);
            } else {
                // Accumulate color from multiple samples
                float r = 0, g = 0, b = 0;
                
                for (int sy = 0; sy < samplesPerPixel; ++sy) {
                    for (int sx = 0; sx < samplesPerPixel; ++sx) {
                        // Calculate subpixel offsets
                        float dx = (sx + static_cast<float>(rand()) / RAND_MAX) / samplesPerPixel;  // Add randomness
                        float dy = (sy + static_cast<float>(rand()) / RAND_MAX) / samplesPerPixel;  // Add randomness
                        
                        Ray ray = camera.generateRay(x + dx, y + dy);
                        Color sample = renderPixel(ray, 0);
                        
                        r += sample.getRed();
                        g += sample.getGreen();
                        b += sample.getBlue();
                    }
                }
                
                // Average the samples
                float scale = 1.0f / (samplesPerPixel * samplesPerPixel);
                pixelColors[y][x] = Color(
                    static_cast<unsigned char>(std::min(r * scale, 255.0f)),  // Add clamping
                    static_cast<unsigned char>(std::min(g * scale, 255.0f)),  // Add clamping
                    static_cast<unsigned char>(std::min(b * scale, 255.0f))   // Add clamping
                );
            }
            // Update progress every 10% // Update progress every 10%
            pixelsCompleted++;
            int currentPercentage = (pixelsCompleted * 100) / totalPixels;
            // Only print at 10% intervals
            if (currentPercentage >= lastPercentage + 10) {
                std::cout << "Rendering: " << currentPercentage << "% complete" << std::endl;
                lastPercentage = currentPercentage;
            }
        }            
    }    
    
    return pixelColors;
}



std::string Renderer::toString() const {
    try {
        std::ostringstream oss;
        // Basic renderer settings
        oss << "Renderer Settings:\n"
            << "Number of bounces: " << nbounces << "\n"
            << "Render Mode: " << (rendermode == RenderMode::BINARY ? "Binary" : "Phong") << "\n\n";

        // Camera settings (safely)
        oss << "Camera Settings:\n"
            << "Width: " << camera.getWidth() << "\n"
            << "Height: " << camera.getHeight() << "\n"
            << "FOV: " << camera.getFOV() << "\n"
            << "Exposure: " << camera.getExposure() << "\n\n";

        // Scene settings (safely)
        oss << "Scene Settings:\n"
            << "Number of Shapes: " << scene.getShapes().size() << "\n"
            << "Number of Lights: " << scene.getLightSources().size() << "\n"
            << "Background Color: ["
            << scene.getBackgroundColor()[0] << ", "
            << scene.getBackgroundColor()[1] << ", "
            << scene.getBackgroundColor()[2] << "]\n";

        return oss.str();
    }
    catch (const std::exception& e) {
        return "Error generating renderer string: " + std::string(e.what());
    }
}
Color Renderer::toneMap(const Color& hdrColor) const{
    float r = hdrColor.getRed() / 255.0f;
    float g = hdrColor.getGreen() / 255.0f;
    float b = hdrColor.getBlue() / 255.0f;

    // Calculate luminance
    float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;

    // Apply tone mapping
    float toneMappedLuminance = luminance / (1 + luminance);
    float scale = toneMappedLuminance / (luminance > 0.0f ? luminance : 1.0f);
    
    // Apply scale and clamp
    r = std::clamp(r * scale * 255.0f, 0.0f, 255.0f);
    g = std::clamp(g * scale * 255.0f, 0.0f, 255.0f);
    b = std::clamp(b * scale * 255.0f, 0.0f, 255.0f);

    return Color(static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b));
}


Color Renderer::renderPixel(const Ray& ray, int currentBounce) {
    
    float minDistance = std::numeric_limits<float>::infinity();
    Color pixelColor(0, 0, 0);
    std::shared_ptr<Shape> closestShape = nullptr;
    Vector3 intersectionPoint;
    Vector3 normal;

    if(!useBVH){
        // Find the closest intersection point
        for (const auto& shape : scene.getShapes()) {
            float distance;
            if (shape->intersect(ray, distance) && distance < minDistance) {
                minDistance = distance;
                closestShape = shape;
                intersectionPoint = ray.getOrigin() + ray.getDirection() * distance;
                normal = shape->getNormal(intersectionPoint);
            }
        } 
    } else {
        float distance;
        auto bvh = scene.getBVH();
        if (bvh) {        
            try {
                Vector3 rayOrigin = ray.getOrigin();
                Vector3 rayDir = ray.getDirection();
                
                bool intersected = bvh->intersect(ray, distance, closestShape);
                if (intersected && distance < minDistance) {
                    minDistance = distance;
                    intersectionPoint = ray.getOrigin() + ray.getDirection() * distance;
                     if (closestShape) {
                         normal = closestShape->getNormal(intersectionPoint);
                     }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error during BVH intersection: " << e.what() << std::endl;
            }
        }
    }

    if (closestShape) {
        Material material = closestShape->getMaterial();
        if (rendermode == RenderMode::BINARY) {
            pixelColor = Color(0, 255, 0);
        } else if (rendermode == RenderMode::PHONG) {   
            Color baseColor;
            if (material.hasTexture) {
                float u, v;
                closestShape->getUVCoordinates(intersectionPoint, u, v);
                baseColor = material.getTexture()->getColorAt(u, v);
            } else {
                baseColor = Color::fromFloatArray(material.diffusecolor);
            }
            Color ambientColor = baseColor * 0.5f;
            Color diffuseColor(0, 0, 0);
            Color specularColor(0, 0, 0);
            
            Vector3 viewDir = ray.getDirection().normalize() * -1;

            for (const auto& light : scene.getLightSources()) {
                Vector3 lightPos = Vector3::fromArray(light->getPosition());
                Vector3 lightDir = (lightPos - intersectionPoint).normalize();
                Ray shadowRay(intersectionPoint + lightDir * 1e-4, lightDir);
                
                bool inShadow = false;
                if (!useBVH) {
                    for (const auto& shadowShape : scene.getShapes()) {
                        float shadowDistance;
                        if (shadowShape->intersect(shadowRay, shadowDistance) &&
                            shadowDistance < (lightPos - intersectionPoint).length()) {
                            inShadow = true;
                            break;
                        }
                    }
                } else {
                    float shadowDistance;
                    std::shared_ptr<Shape> shadowShape;
                    auto bvh = scene.getBVH();
                    if (bvh && bvh->intersect(shadowRay, shadowDistance, shadowShape)) {
                        if (shadowDistance < (lightPos - intersectionPoint).length()) {
                            inShadow = true;
                        }
                    }
                }
                if (!inShadow) {
                    Vector3 halfDir = (viewDir + lightDir).normalize();

                    float diff = std::max(normal.dot(lightDir), 0.0f);
                    diffuseColor += Color::fromFloatArray(material.diffusecolor) * diff * material.kd * Color::fromFloatArray(light->getIntensity());

                    float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularexponent);
                    specularColor += Color::fromFloatArray(material.specularcolor) * spec * material.ks * Color::fromFloatArray(light->getIntensity());
                }
            }
            
            pixelColor = ambientColor + diffuseColor + specularColor;
            //std::cout << "Calculating reflectivity" << std::endl;
            if (material.isreflective && currentBounce < nbounces) {
                Vector3 reflectDir = ray.getDirection() - (normal * ray.getDirection().dot(normal) * 2.0f);
                Ray reflectedRay(intersectionPoint + reflectDir * 1e-4, reflectDir);

                Color reflectedColor = renderPixel(reflectedRay, currentBounce + 1);
                pixelColor = pixelColor * (1 - material.reflectivity) + reflectedColor * material.reflectivity;
            }

            // Refraction logic
            //std::cout << "Calculating refraction" << std::endl;
            if (material.isrefractive && currentBounce < nbounces) {
                float eta = 1.0f; // Assumed to be the refractive index of air
                float etaPrime = material.refractiveindex;
                float cosThetaI = -normal.dot(ray.getDirection());

                if (cosThetaI < 0) {
                    cosThetaI = -cosThetaI;
                    normal = normal * -1;
                    std::swap(eta, etaPrime);
                }

                float etaRatio = eta / etaPrime;
                float cosThetaT2 = 1.0f - etaRatio * etaRatio * (1.0f - cosThetaI * cosThetaI);

                if (cosThetaT2 > 0.0f) {
                    Vector3 refractionDir = (ray.getDirection() * etaRatio + normal * (etaRatio * cosThetaI - sqrt(cosThetaT2))).normalize();
                    Ray refractedRay(intersectionPoint - normal * 1e-4, refractionDir);

                    Color refractedColor = renderPixel(refractedRay, currentBounce + 1);
                    pixelColor = pixelColor * (1 - material.reflectivity) + refractedColor * material.reflectivity;
                }
            }
            pixelColor = pixelColor.clamp();
        }
    }

    return pixelColor;
}

