#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
// Color Renderer::toneMap(const Color& hdrColor) const{
//     float r = hdrColor.getRed() / 255.0f;
//     float g = hdrColor.getGreen() / 255.0f;
//     float b = hdrColor.getBlue() / 255.0f;

//     // Calculate luminance
//     float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;

//     // Apply tone mapping
//     float toneMappedLuminance = luminance / (1 + luminance);
//     float scale = toneMappedLuminance / (luminance > 0.0f ? luminance : 1.0f);
    
//     // Apply scale and clamp
//     r = std::clamp(r * scale * 255.0f, 0.0f, 255.0f);
//     g = std::clamp(g * scale * 255.0f, 0.0f, 255.0f);
//     b = std::clamp(b * scale * 255.0f, 0.0f, 255.0f);

//     return Color(static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b));
// }

Color Renderer::toneMap(const Color& hdrColor) const {
    // Convert to floating point and apply exposure boost
    float exposure = camera.getExposure() * 2.0f;  // Increase exposure
    float r = (hdrColor.getRed() / 255.0f);
    float g = (hdrColor.getGreen() / 255.0f);
    float b = (hdrColor.getBlue() / 255.0f);

    // Calculate luminance with adjusted weights
    float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;

    // Adjust white point to preserve bright areas
    float whitePoint = 1.2f;  // Increase this value to preserve more brightness
    float toneMappedLuminance = (luminance * (1.0f + luminance / (whitePoint * whitePoint))) / (1.0f + luminance);
    
    // Calculate scale while preventing division by zero
    float scale = luminance > 0.0f ? toneMappedLuminance / luminance : 1.0f;
    
    // Apply scale and convert back to [0,255] range
    r = std::clamp(r * scale * 255.0f, 0.0f, 255.0f);
    g = std::clamp(g * scale * 255.0f, 0.0f, 255.0f);
    b = std::clamp(b * scale * 255.0f, 0.0f, 255.0f);

    return Color(
        static_cast<unsigned char>(r),
        static_cast<unsigned char>(g),
        static_cast<unsigned char>(b)
    );
}

// Color Renderer::toneMap(const Color& hdrColor) const {
//     // Convert color to floating point values in [0,1]
//     float r = hdrColor.getRed() / 255.0f;
//     float g = hdrColor.getGreen() / 255.0f;
//     float b = hdrColor.getBlue() / 255.0f;
    
//     // Apply exposure adjustment
//     float exposure = camera.getExposure();
//     r *= exposure;
//     g *= exposure;
//     b *= exposure;
    
//     // Reinhard tone mapping operator
//     r = r / (1.0f + r);
//     g = g / (1.0f + g);
//     b = b / (1.0f + b);
    
//     // Convert back to 0-255 range
//     return Color(
//         static_cast<unsigned char>(std::min(r * 255.0f, 255.0f)),
//         static_cast<unsigned char>(std::min(g * 255.0f, 255.0f)),
//         static_cast<unsigned char>(std::min(b * 255.0f, 255.0f))
//     );
// }

// Color Renderer::toneMap(const Color& hdrColor) const {
//     // Convert to floating point
//     float r = hdrColor.getRed() / 255.0f;
//     float g = hdrColor.getGreen() / 255.0f;
//     float b = hdrColor.getBlue() / 255.0f;
    
//     // Apply exposure boost
//     float exposure = camera.getExposure() * 10.0f;  // Increase exposure
//     r *= exposure;
//     g *= exposure;
//     b *= exposure;
    
//     // Modified Reinhard operator that preserves more brightness
//     float white = 2.0f;  // Higher value = brighter whites
//     r = (r * (1.0f + r / (white * white))) / (1.0f + r);
//     g = (g * (1.0f + g / (white * white))) / (1.0f + g);
//     b = (b * (1.0f + b / (white * white))) / (1.0f + b);
    
//     // Convert back to [0,255] range
//     return Color(
//         static_cast<unsigned char>(std::min(r * 255.0f, 255.0f)),
//         static_cast<unsigned char>(std::min(g * 255.0f, 255.0f)),
//         static_cast<unsigned char>(std::min(b * 255.0f, 255.0f))
//     );
// }



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
            Color ambientColor = baseColor* 0.5;// * 0.5f;
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
        } else if (rendermode == RenderMode::PATHTRACER) {
            // Accumulate colors as floats to avoid premature clamping
            float r = 0.0f, g = 0.0f, b = 0.0f;
            int samples = samplesPerPixel * samplesPerPixel;
            
            for (int i = 0; i < samples; ++i) {
                Color sample = tracePath(ray, 0);
                r += sample.getRed() / 255.0f;    // Convert to float in [0,1] range
                g += sample.getGreen() / 255.0f;
                b += sample.getBlue() / 255.0f;
            }
            
            // Average the samples and convert back to [0,255] range
            float scale = 1.0f / samples;
            Color finalColor(
                static_cast<unsigned char>(std::min(r * scale * 255.0f, 255.0f)),
                static_cast<unsigned char>(std::min(g * scale * 255.0f, 255.0f)),
                static_cast<unsigned char>(std::min(b * scale * 255.0f, 255.0f))
            );
            return finalColor;
        }
    }

    return toneMap(pixelColor);
    //return pixelColor;
}

Color Renderer::tracePath(const Ray& ray, int depth) {
    if (depth >= nbounces) {
        return Color(0, 0, 0);
    }

    float minDistance = std::numeric_limits<float>::infinity();
    std::shared_ptr<Shape> closestShape = nullptr;
    Vector3 intersectionPoint;
    Vector3 normal;

    // Use existing intersection code
    if (!useBVH) {
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
        if (bvh && bvh->intersect(ray, distance, closestShape)) {
            if (distance < minDistance) {
                minDistance = distance;
                intersectionPoint = ray.getOrigin() + ray.getDirection() * distance;
                normal = closestShape->getNormal(intersectionPoint);
            }
        }
    }

    if (!closestShape) {
        return Color::fromFloatArray(scene.getBackgroundColor());
    }

    Material material = closestShape->getMaterial();
    Color emittance(0, 0, 0);  // For future support of emissive materials

    // Russian Roulette termination
    if (depth > 5) {
        float continueProbability = 0.5f;
        if (randomFloat() > continueProbability) {
            return emittance;
        }
    }

    // Handle reflections
    if (material.isreflective) {
        float reflectProb = material.reflectivity;
        if (randomFloat() < reflectProb) {
            Vector3 reflectDir = ray.getDirection() - 
                               (normal * 2.0f * ray.getDirection().dot(normal));
            Ray reflectedRay(intersectionPoint + reflectDir * 1e-4f, reflectDir);
            Color reflectedColor = tracePath(reflectedRay, depth + 1);
            
            // Get material color
            Color materialColor = material.hasTexture ? 
                material.getTexture()->getColorAt(0, 0) : // Implement proper UV mapping
                Color::fromFloatArray(material.specularcolor);
                
            // Modulate reflected color by material color and reflectivity
            return reflectedColor * materialColor * (1.0f / reflectProb);
        }
    }

    // Handle refractions
    if (material.isrefractive) {
        float eta = 1.0f;  // Air refractive index
        float etaPrime = material.refractiveindex;
        float cosThetaI = -normal.dot(ray.getDirection());
        
        // Handle ray entering or exiting medium
        if (cosThetaI < 0) {
            cosThetaI = -cosThetaI;
            normal = normal * -1;
            std::swap(eta, etaPrime);
        }

        float etaRatio = eta / etaPrime;
        float cosThetaT2 = 1.0f - etaRatio * etaRatio * (1.0f - cosThetaI * cosThetaI);

        if (cosThetaT2 > 0.0f) {
            // Calculate Fresnel terms
            float r0 = ((eta - etaPrime) / (eta + etaPrime)) * 
                      ((eta - etaPrime) / (eta + etaPrime));
            float fresnel = r0 + (1.0f - r0) * pow(1.0f - cosThetaI, 5.0f);

            // Probabilistically choose reflection or refraction
            if (randomFloat() < fresnel) {
                // Reflection
                Vector3 reflectDir = ray.getDirection() - 
                                   (normal * 2.0f * ray.getDirection().dot(normal));
                Ray reflectedRay(intersectionPoint + reflectDir * 1e-4f, reflectDir);
                return tracePath(reflectedRay, depth + 1) * (1.0f / fresnel);
            } else {
                // Refraction
                Vector3 refractionDir = (ray.getDirection() * etaRatio + 
                    normal * (etaRatio * cosThetaI - sqrt(cosThetaT2))).normalize();
                Ray refractedRay(intersectionPoint - normal * 1e-4f, refractionDir);
                return tracePath(refractedRay, depth + 1) * (1.0f / (1.0f - fresnel));
            }
        }
        // Total internal reflection
        Vector3 reflectDir = ray.getDirection() - 
                           (normal * 2.0f * ray.getDirection().dot(normal));
        Ray reflectedRay(intersectionPoint + reflectDir * 1e-4f, reflectDir);
        return tracePath(reflectedRay, depth + 1);
    }

    // Diffuse surface handling (original path tracing code)
    Vector3 newDirection = randomHemisphereDirection(normal);
    Ray newRay(intersectionPoint + newDirection * 1e-4f, newDirection);

    Color incomingLight = tracePath(newRay, depth + 1);

    // Calculate BRDF
    Color albedo = material.hasTexture ? 
        material.getTexture()->getColorAt(0, 0) : 
        Color::fromFloatArray(material.diffusecolor);

    float cosTheta = std::max(0.0f, normal.dot(newDirection));
    
    // Direct lighting calculation
    Color directLight(0, 0, 0);
    for (const auto& light : scene.getLightSources()) {
        Vector3 lightPos = Vector3::fromArray(light->getPosition());
        Vector3 lightDir = (lightPos - intersectionPoint).normalize();
        
        Ray shadowRay(intersectionPoint + lightDir * 1e-4f, lightDir);
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
            if (scene.getBVH()->intersect(shadowRay, shadowDistance, shadowShape)) {
                if (shadowDistance < (lightPos - intersectionPoint).length()) {
                    inShadow = true;
                }
            }
        }
        
        if (!inShadow) {
            float lightCosTheta = std::max(0.0f, normal.dot(lightDir));
            directLight = directLight + Color::fromFloatArray(light->getIntensity()) * lightCosTheta;
        }
    }

    return emittance + albedo * (directLight + incomingLight * cosTheta) * (1.0f / M_PI);
}

Vector3 Renderer::randomHemisphereDirection(const Vector3& normal){
    float u1 = randomFloat();
    float u2 = randomFloat();
    
    float r = std::sqrt(1.0f - u1 * u1);
    float phi = 2.0f * M_PI * u2;
    
    // Change normal.x() to normal.x
    Vector3 tangent = std::abs(normal.x) > 0.9f ? 
        Vector3(0, 1, 0) : Vector3(1, 0, 0);
    
    Vector3 bitangent = normal.cross(tangent).normalize();
    tangent = bitangent.cross(normal);
    
    return (tangent * (r * std::cos(phi)) + 
            bitangent * (r * std::sin(phi)) + 
            normal * u1).normalize();
}

float Renderer::randomFloat() {
    return static_cast<float>(rand()) / RAND_MAX;
}
