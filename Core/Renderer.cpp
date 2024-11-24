#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "Renderer.h"
#include <sstream>
#include <limits> 
#include <fstream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <system_error>

#include "Ray.h"
#include "../Geometry/Vector3.h"
#include "../Geometry/Shape.h"
#include "../Materials/Material.h"
#include "../Materials/Color.h"
#include "../Lighting/Light.h"

// Constructor 
Renderer::Renderer(int nbounces, RenderMode rendermode, const Camera& camera, const Scene& scene, bool useBVH, bool useAntiAliasing, int samplesPerPixel)
    : nbounces(nbounces), rendermode(rendermode), camera(camera), scene(scene), useBVH(useBVH), useAntiAliasing(useAntiAliasing), samplesPerPixel(samplesPerPixel) {}

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

// Method to render the scene and return a 2D array of pixel colors
std::vector<std::vector<Color>> Renderer::renderScene() {
    // Get dimensions from camera
    int width = camera.getWidth();
    int height = camera.getHeight();

    // Calculate total number of pixels
    int totalPixels = width * height;

    // Initialize variables for progress tracking
    int pixelsCompleted = 0;
    int lastPercentage = -10;

    // Create the pixel color buffer
    std::vector<std::vector<Color>> pixelColors(height, std::vector<Color>(width));


    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            // If anti-aliasing is not used, generate a single ray for the pixel
            if (!useAntiAliasing) {
                Ray ray = camera.generateRay(x, y);
                pixelColors[y][x] = renderPixel(ray, 0);
            } else {    
                // If anti-aliasing is used, accumulate color from multiple samples
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
            // Update progress every 10%
            pixelsCompleted++;
            int currentPercentage = (pixelsCompleted * 100) / totalPixels;
            if (currentPercentage >= lastPercentage + 10) {
                std::cout << "Rendering: " << currentPercentage << "% complete" << std::endl;
                lastPercentage = currentPercentage;
            }
        }            
    }    
    
    return pixelColors;
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


//------------- GOOD METHOD ----------------
// Color Renderer::renderPixel(const Ray& ray, int currentBounce) {
    
//     float minDistance = std::numeric_limits<float>::infinity();
//     Color pixelColor(0, 0, 0);
//     std::shared_ptr<Shape> closestShape = nullptr;
//     Vector3 intersectionPoint;
//     Vector3 normal;

//     if(!useBVH){
//         // Find the closest intersection point
//         for (const auto& shape : scene.getShapes()) {
//             float distance;
//             if (shape->intersect(ray, distance) && distance < minDistance) {
//                 minDistance = distance;
//                 closestShape = shape;
//                 intersectionPoint = ray.getOrigin() + ray.getDirection() * distance;
//                 normal = shape->getNormal(intersectionPoint);
//             }
//         } 
//     } else {
//         float distance;
//         auto bvh = scene.getBVH();
//         if (bvh) {        
//             try {
//                 Vector3 rayOrigin = ray.getOrigin();
//                 Vector3 rayDir = ray.getDirection();
                
//                 bool intersected = bvh->intersect(ray, distance, closestShape);
//                 if (intersected && distance < minDistance) {
//                     minDistance = distance;
//                     intersectionPoint = ray.getOrigin() + ray.getDirection() * distance;
//                      if (closestShape) {
//                          normal = closestShape->getNormal(intersectionPoint);
//                      }
//                 }
//             } catch (const std::exception& e) {
//                 std::cerr << "Error during BVH intersection: " << e.what() << std::endl;
//             }
//         }
//     }

//     if (closestShape) {
//         Material material = closestShape->getMaterial();
//         if (rendermode == RenderMode::BINARY) {
//             pixelColor = Color(255, 0, 0);
//         } else if (rendermode == RenderMode::PHONG) {   
//             Color baseColor;
//             if (material.hasTexture) {
//                 float u, v;
//                 closestShape->getUVCoordinates(intersectionPoint, u, v);
//                 baseColor = material.getTexture()->getColorAt(u, v);
//             } else {
//                 baseColor = Color::fromFloatArray(material.diffusecolor);
//             }
//             Color ambientColor = baseColor* 0.5;
//             Color diffuseColor(0, 0, 0);
//             Color specularColor(0, 0, 0);
            
//             Vector3 viewDir = ray.getDirection().normalize() * -1;

//             for (const auto& light : scene.getLightSources()) {
//                 Vector3 lightPos = Vector3::fromArray(light->getPosition());
//                 Vector3 lightDir = (lightPos - intersectionPoint).normalize();
//                 Ray shadowRay(intersectionPoint + lightDir * 1e-4, lightDir);
                
//                 bool inShadow = false;
//                 if (!useBVH) {
//                     for (const auto& shadowShape : scene.getShapes()) {
//                         float shadowDistance;
//                         if (shadowShape->intersect(shadowRay, shadowDistance) &&
//                             shadowDistance < (lightPos - intersectionPoint).length()) {
//                             inShadow = true;
//                             break;
//                         }
//                     }
//                 } else {
//                     float shadowDistance;
//                     std::shared_ptr<Shape> shadowShape;
//                     auto bvh = scene.getBVH();
//                     if (bvh && bvh->intersect(shadowRay, shadowDistance, shadowShape)) {
//                         if (shadowDistance < (lightPos - intersectionPoint).length()) {
//                             inShadow = true;
//                         }
//                     }
//                 }
//                 if (!inShadow) {
//                     Vector3 halfDir = (viewDir + lightDir).normalize();

//                     float diff = std::max(normal.dot(lightDir), 0.0f);
//                     diffuseColor += Color::fromFloatArray(material.diffusecolor) * diff * material.kd * Color::fromFloatArray(light->getIntensity());

//                     float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularexponent);
//                     specularColor += Color::fromFloatArray(material.specularcolor) * spec * material.ks * Color::fromFloatArray(light->getIntensity());
//                 }
//             }
            
//             pixelColor = ambientColor + diffuseColor + specularColor;
//             if (material.isreflective && currentBounce < nbounces) {
//                 Vector3 reflectDir = ray.getDirection() - (normal * ray.getDirection().dot(normal) * 2.0f);
//                 Ray reflectedRay(intersectionPoint + reflectDir * 1e-4, reflectDir);

//                 Color reflectedColor = renderPixel(reflectedRay, currentBounce + 1);
//                 pixelColor = pixelColor * (1 - material.reflectivity) + reflectedColor * material.reflectivity;
//             }

//             //Refraction logic
//             if (material.isrefractive && currentBounce < nbounces) {
//                 float eta = 1.0f; // Assumed to be the refractive index of air
//                 float etaPrime = material.refractiveindex;
//                 float cosThetaI = -normal.dot(ray.getDirection());

//                 if (cosThetaI < 0) {
//                     cosThetaI = -cosThetaI;
//                     normal = normal * -1;
//                     std::swap(eta, etaPrime);
//                 }

//                 float etaRatio = eta / etaPrime;
//                 float cosThetaT2 = 1.0f - etaRatio * etaRatio * (1.0f - cosThetaI * cosThetaI);

//                 if (cosThetaT2 > 0.0f) {
//                     Vector3 refractionDir = (ray.getDirection() * etaRatio + normal * (etaRatio * cosThetaI - sqrt(cosThetaT2))).normalize();
//                     Ray refractedRay(intersectionPoint - normal * 1e-4, refractionDir);

//                     Color refractedColor = renderPixel(refractedRay, currentBounce + 1);
//                     pixelColor = pixelColor * (1 - material.reflectivity) + refractedColor * material.reflectivity;
//                 }
//             }
//             pixelColor = pixelColor.clamp();
//         } else if (rendermode == RenderMode::PATHTRACER) {
//             // Accumulate colors as floats to avoid premature clamping
//             float r = 0.0f, g = 0.0f, b = 0.0f;
//             int ptSamplesPerPixel = 4;
//             int samples = ptSamplesPerPixel * ptSamplesPerPixel;
            
//             for (int i = 0; i < samples; ++i) {
//                 Color sample = tracePath(ray, 0);
//                 r += sample.getRed() / 255.0f;    // Convert to float in [0,1] range
//                 g += sample.getGreen() / 255.0f;
//                 b += sample.getBlue() / 255.0f;
//             }
            
//             // Average the samples and convert back to [0,255] range
//             float scale = 1.0f / samples;
//             Color finalColor(
//                 static_cast<unsigned char>(std::min(r * scale * 255.0f, 255.0f)),
//                 static_cast<unsigned char>(std::min(g * scale * 255.0f, 255.0f)),
//                 static_cast<unsigned char>(std::min(b * scale * 255.0f, 255.0f))
//             );
//             return finalColor;
//         }
//     }

//     //return toneMap(pixelColor);
//     return pixelColor;
// } 

Color Renderer::renderPixel(const Ray& ray, int currentBounce) {
    // Render based on the selected render mode
    if (rendermode == RenderMode::BINARY) {
        return renderBinary(ray, currentBounce);
    } else if (rendermode == RenderMode::PHONG) {
        return renderBlinnPhong(ray, currentBounce);
    } else if (rendermode == RenderMode::PATHTRACER) {
        return renderPathTracer(ray, currentBounce);
    }
    return Color(0, 0, 0);
}

Color Renderer::renderBinary(const Ray& ray, int currentBounce) {
    // We only care about if there is an intersection but initialise values so we can use findClosestIntersection
    float minDistance = std::numeric_limits<float>::infinity();
    std::shared_ptr<Shape> closestShape = nullptr;
    Vector3 intersectionPoint;
    Vector3 normal;
    if (findClosestIntersection(ray, minDistance, closestShape, intersectionPoint, normal)) {
        return Color(255, 0, 0);
    } else {
        return Color(0, 0, 0);
    }
}

Color Renderer::renderBlinnPhong(const Ray& ray, int currentBounce) {
    // Initialise values 
    float minDistance = std::numeric_limits<float>::infinity();
    std::shared_ptr<Shape> closestShape = nullptr;
    Vector3 intersectionPoint;
    Vector3 normal;

    // If there is no intersection, return black
    // findClosestIntersection updates all values we need 
    if (!findClosestIntersection(ray, minDistance, closestShape, intersectionPoint, normal)) {
        return Color(0, 0, 0);
    }

    Material material = closestShape->getMaterial();

    // Set base colour to texture if exists, or diffuse colour otherwise 
    Color baseColor;
    if (material.hasTexture) {
        float u, v;
        closestShape->getUVCoordinates(intersectionPoint, u, v);
        baseColor = material.getTexture()->getColorAt(u, v);
    } else {
        baseColor = Color::fromFloatArray(material.diffusecolor);
    }

    // Set ambient colour to 50% of base colour
    Color ambientColor = baseColor * 0.5;
    Color diffuseColor(0, 0, 0);
    Color specularColor(0, 0, 0);
    
    Vector3 viewDir = ray.getDirection().normalize() * -1;

    // Calculate lighting
    for (const auto& light : scene.getLightSources()) {
        // Create a shadow ray 
        Vector3 lightPos = Vector3::fromArray(light->getPosition());
        Vector3 lightDir = (lightPos - intersectionPoint).normalize();
        Ray shadowRay(intersectionPoint + lightDir * 1e-4, lightDir);
        
        // Check if the shadow ray is in shadow
        if (!isInShadow(shadowRay, (lightPos - intersectionPoint).length())) {
            Vector3 halfDir = (viewDir + lightDir).normalize();

            // Calculate diffuse lighting
            float diff = std::max(normal.dot(lightDir), 0.0f);
            diffuseColor += Color::fromFloatArray(material.diffusecolor) * diff * material.kd * 
                           Color::fromFloatArray(light->getIntensity());

            // Calculate specular lighting
            float spec = std::pow(std::max(normal.dot(halfDir), 0.0f), material.specularexponent);
            specularColor += Color::fromFloatArray(material.specularcolor) * spec * material.ks * 
                            Color::fromFloatArray(light->getIntensity());
        }
    }
    
    // Combine ambient, diffuse and specular lighting
    Color pixelColor = ambientColor + diffuseColor + specularColor;

    // Handle reflections
    if (material.isreflective && currentBounce < nbounces) {
        // Calculate reflection direction
        Vector3 reflectDir = ray.getDirection() - (normal * ray.getDirection().dot(normal) * 2.0f);
        // Create a reflected ray
        Ray reflectedRay(intersectionPoint + reflectDir * 1e-4, reflectDir);
        // Calculate reflected colour
        Color reflectedColor = renderPixel(reflectedRay, currentBounce + 1);
        // Combine with reflection properties
        pixelColor = pixelColor * (1 - material.reflectivity) + reflectedColor * material.reflectivity;
    }

    // Handle refraction
    if (material.isrefractive && currentBounce < nbounces) {
        // Set refractive indices of air and material
        float eta = 1.0f;
        float etaPrime = material.refractiveindex;
        // Calculate cosine of the angle between the normal and the ray direction
        float cosThetaI = -normal.dot(ray.getDirection());

        // If the cosine is negative, the ray is exiting the material
        if (cosThetaI < 0) {
            cosThetaI = -cosThetaI;
            normal = normal * -1; // Flip normal
            std::swap(eta, etaPrime); // Swap refractive indices
        }

        // Calculate ratio of refractive indices
        float etaRatio = eta / etaPrime;
        float cosThetaT2 = 1.0f - etaRatio * etaRatio * (1.0f - cosThetaI * cosThetaI);

        // If the square of the cosine is positive, there is refraction
        if (cosThetaT2 > 0.0f) {
            // Calculate refraction direction
            Vector3 refractionDir = (ray.getDirection() * etaRatio + normal * (etaRatio * cosThetaI - sqrt(cosThetaT2))).normalize();
            // Create a refracted ray
            Ray refractedRay(intersectionPoint - normal * 1e-4, refractionDir);
            // Calculate refracted colour
            Color refractedColor = renderPixel(refractedRay, currentBounce + 1);
            // Combine with refraction properties
            pixelColor = pixelColor * (1 - material.reflectivity) + refractedColor * material.reflectivity;
        }
    }

    return pixelColor.clamp();
}

Color Renderer::renderPathTracer(const Ray& ray, int currentBounce) {
    // Accumulate colors as floats to avoid premature clamping
    float r = 0.0f, g = 0.0f, b = 0.0f;
    int ptSamplesPerPixel = 4;
    int samples = ptSamplesPerPixel * ptSamplesPerPixel;
    
    for (int i = 0; i < samples; ++i) {
        Color sample = tracePath(ray, 0);
        r += sample.getRed() / 255.0f;
        g += sample.getGreen() / 255.0f;
        b += sample.getBlue() / 255.0f;
    }
    
    // Average the samples and convert back to [0,255] range
    float scale = 1.0f / samples;
    return Color(
        static_cast<unsigned char>(std::min(r * scale * 255.0f, 255.0f)),
        static_cast<unsigned char>(std::min(g * scale * 255.0f, 255.0f)),
        static_cast<unsigned char>(std::min(b * scale * 255.0f, 255.0f))
    );
}

Color Renderer::tracePath(const Ray& ray, int depth) {
    if (depth >= nbounces) {
        return Color(0, 0, 0);
    }

    float minDistance = std::numeric_limits<float>::infinity();
    std::shared_ptr<Shape> closestShape = nullptr;
    Vector3 intersectionPoint, normal;

    // Intersection logic
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
        if (scene.getBVH()->intersect(ray, distance, closestShape) && distance < minDistance) {
            minDistance = distance;
            intersectionPoint = ray.getOrigin() + ray.getDirection() * distance;
            normal = closestShape->getNormal(intersectionPoint);
        }
    }

    if (!closestShape) {
        // Increase background contribution
        Color bgColor = Color::fromFloatArray(scene.getBackgroundColor());
        return bgColor * 2.0f;  // Multiply background intensity
    }

    Material material = closestShape->getMaterial();
    Color materialColor = material.hasTexture ? 
        material.getTexture()->getColorAt(0, 0) : 
        Color::fromFloatArray(material.diffusecolor);

    if (depth > 3) {
        float continueProbability = 0.9f;
        if (randomFloat() > continueProbability) {
            return materialColor * (1.0f - continueProbability);
        }
    }

    Color directLight(0, 0, 0);
    for (const auto& light : scene.getLightSources()) {
        Vector3 lightDir = (Vector3::fromArray(light->getPosition()) - intersectionPoint).normalize();
        float lightDistance = (Vector3::fromArray(light->getPosition()) - intersectionPoint).length();

        Ray shadowRay(intersectionPoint + lightDir * 1e-4f, lightDir);
        bool inShadow = false;

        if (!useBVH) {
            for (const auto& shadowShape : scene.getShapes()) {
                float shadowDistance;
                if (shadowShape->intersect(shadowRay, shadowDistance) && shadowDistance < lightDistance) {
                    inShadow = true;
                    break;
                }
            }
        } else {
            float shadowDistance;
            std::shared_ptr<Shape> shadowShape;
            if (scene.getBVH()->intersect(shadowRay, shadowDistance, shadowShape) && shadowDistance < lightDistance) {
                inShadow = true;
            }
        }

        if (!inShadow) {
            float lightCosTheta = std::max(0.0f, normal.dot(lightDir));
            float attenuation = 1.0f / (lightDistance * lightDistance);
            directLight = directLight + Color::fromFloatArray(light->getIntensity()) * lightCosTheta * attenuation * 2.0f;
        }
    }

    Vector3 newDirection = randomHemisphereDirection(normal);
    Ray newRay(intersectionPoint + newDirection * 1e-4f, newDirection);
    Color indirectLight = tracePath(newRay, depth + 1);

    float cosTheta = std::max(0.0f, normal.dot(newDirection));
    Color finalColor = materialColor * (directLight + indirectLight * cosTheta);

    if (material.isreflective) {
        // Reflection
        Vector3 reflectDir = ray.getDirection() - normal * 2.0f * ray.getDirection().dot(normal);
        Ray reflectedRay(intersectionPoint + reflectDir * 1e-4f, reflectDir);
        Color reflectedColor = tracePath(reflectedRay, depth + 1);
        
        // Fresnel approximation
        float fresnel = material.reflectivity + (1.0f - material.reflectivity) * 
                       pow(1.0f - std::abs(normal.dot(ray.getDirection())), 5.0f);
        
        finalColor = finalColor * (1.0f - fresnel) + reflectedColor * fresnel;
    }

    return finalColor;
}

bool Renderer::findClosestIntersection(const Ray& ray, float& minDistance, 
                                     std::shared_ptr<Shape>& closestShape,
                                     Vector3& intersectionPoint, Vector3& normal) {
    minDistance = std::numeric_limits<float>::infinity();
    closestShape = nullptr;

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
        if (bvh) {
            try {
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

    return closestShape != nullptr;
}

bool Renderer::isInShadow(const Ray& shadowRay, float lightDistance) {
    if (!useBVH) {
        for (const auto& shadowShape : scene.getShapes()) {
            float shadowDistance;
            if (shadowShape->intersect(shadowRay, shadowDistance) && 
                shadowDistance < lightDistance) {
                return true;
            }
        }
    } else {
        float shadowDistance;
        std::shared_ptr<Shape> shadowShape;
        auto bvh = scene.getBVH();
        if (bvh && bvh->intersect(shadowRay, shadowDistance, shadowShape) && 
            shadowDistance < lightDistance) {
            return true;
        }
    }
    return false;
}

Vector3 Renderer::randomHemisphereDirection(const Vector3& normal) {
    float u1 = randomFloat();
    float u2 = randomFloat();
    
    float r = std::sqrt(1.0f - u1 * u1);
    float phi = 2.0f * M_PI * u2;
    
    Vector3 tangent = std::abs(normal.x) > 0.9f ? 
        Vector3(0, 1, 0) : Vector3(1, 0, 0);
    
    Vector3 bitangent = normal.cross(tangent).normalize();
    tangent = bitangent.cross(normal);
    
    return (tangent * (r * std::cos(phi)) + 
            bitangent * (r * std::sin(phi)) + 
            normal * u1).normalize();
}

float Renderer::randomFloat() {
    return rand() / (RAND_MAX + 1.0f);
}

Color Renderer::toneMap(const Color& hdrColor) const {  
    float r = (hdrColor.getRed() / 255.0f);
    float g = (hdrColor.getGreen() / 255.0f);
    float b = (hdrColor.getBlue() / 255.0f);

    // Calculate luminance with adjusted weights
    float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;

    // Adjust white point to preserve bright areas
    float whitePoint = 0.8f;  // Increase this value to preserve more brightness
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