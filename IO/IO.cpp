#include "IO.h"
#include <fstream>
#include <stdexcept>
#include "../Geometry/Sphere.h"
#include "../Geometry/Cylinder.h"
#include "../Geometry/Triangle.h"
#include "../Lighting/PointLight.h"
#include "../Lighting/AreaLight.h"

Renderer IO::loadRenderer(const std::string& filename, bool useBVH, bool useAntiAliasing, int samplesPerPixel) {
    // Load the renderer from a JSON file
    std::string filepath = "sceneJsons/" + filename;
    json j = parseJSONFile(filepath);

    try {
        // Check if required fields exist
        if (!j.contains("rendermode")) {
            throw std::runtime_error("JSON missing 'rendermode' field");
        }
        if (!j.contains("camera")) {
            throw std::runtime_error("JSON missing 'camera' field");
        }
        if (!j.contains("scene")) {
            throw std::runtime_error("JSON missing 'scene' field");
        }

        // Parse number of bounces
        int nbounces = 0;
        if (j.contains("nbounces")) {
            nbounces = j["nbounces"];
        }

        // Parse render mode
        std::string renderModeStr = j["rendermode"];
        Renderer::RenderMode rendermode = parseRenderMode(renderModeStr);

        // Parse camera
        Camera camera = parseCamera(j["camera"]);

        // Parse scene
        Scene scene = loadSceneFromJSON(j["scene"]);

        // Create and return renderer
        return Renderer(nbounces, rendermode, camera, scene, useBVH, useAntiAliasing, samplesPerPixel);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error in loadRendererDetails: " + std::string(e.what()));
    }
}

// Parse a JSON file
json IO::parseJSONFile(const std::string& filepath) {
    // Open the file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }

    try {
        // Parse the JSON file
        json j;
        file >> j;
        return j;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
    }
}

// Parse a material from JSON data
Material IO::parseMaterial(const json& materialData) {
    // Check if the material has a texture
    bool hasTexture = false;
    std::string textureFilename = "";
    if (materialData.contains("hasTexture")) {
        hasTexture = materialData["hasTexture"];
    }
    // Check if the material has a texture filename
    if (materialData.contains("textureFilename")) {
        textureFilename = materialData["textureFilename"];
    }

    // Create and return the material
    return Material(
        materialData["ks"],
        materialData["kd"],
        materialData["specularexponent"],
        {materialData["diffusecolor"][0], materialData["diffusecolor"][1], materialData["diffusecolor"][2]},
        {materialData["specularcolor"][0], materialData["specularcolor"][1], materialData["specularcolor"][2]},
        materialData["isreflective"],
        materialData["reflectivity"],
        materialData["isrefractive"],
        materialData["refractiveindex"],
        hasTexture,
        textureFilename
    );
}

// Parse a camera from JSON data
Camera IO::parseCamera(const json& cameraData) {
    // Parse the camera type
    Camera::CameraType type = Camera::CameraType::PINHOLE;
    if (cameraData["type"] == "thinlens") {
        type = Camera::CameraType::THIN_LENS;
    }
    
    float aperture = 0.0f;
    float focalDistance = 0.0f;
    
    // Check if the camera has an aperture
    if (cameraData.contains("aperture")) {
        aperture = cameraData["aperture"];
    }
    // Check if the camera has a focal distance
    if (cameraData.contains("focalDistance")) {
        focalDistance = cameraData["focalDistance"];
    }

    // Create and return the camera
    return Camera(
        type,
        cameraData["width"],
        cameraData["height"],
        {cameraData["position"][0], cameraData["position"][1], cameraData["position"][2]},
        {cameraData["lookAt"][0], cameraData["lookAt"][1], cameraData["lookAt"][2]},
        {cameraData["upVector"][0], cameraData["upVector"][1], cameraData["upVector"][2]},
        cameraData["fov"],
        cameraData["exposure"],
        aperture,
        focalDistance
    );
}

std::vector<std::shared_ptr<Shape>> IO::loadShapesFromJSON(const json& sceneData) {
    // Create a vector to store the shapes
    std::vector<std::shared_ptr<Shape>> shapes;

    // Loop through the shapes in the scene
    for (const auto& shapeData : sceneData["shapes"]) {
        // Parse the shape type
        std::string type = shapeData["type"];

        // Parse the material
        Material material;
        if (shapeData.contains("material")) {
            material = parseMaterial(shapeData["material"]);
        }

        // Create the shape based on the type
        if (type == "sphere") {
            Vector3 center = {shapeData["center"][0], shapeData["center"][1], shapeData["center"][2]};
            shapes.push_back(std::make_shared<Sphere>(center, shapeData["radius"], material));
        } 
        else if (type == "cylinder") {
            Vector3 center = {shapeData["center"][0], shapeData["center"][1], shapeData["center"][2]};
            Vector3 axis = {shapeData["axis"][0], shapeData["axis"][1], shapeData["axis"][2]};
            shapes.push_back(std::make_shared<Cylinder>(center, axis, shapeData["radius"], shapeData["height"], material));
        } 
        else if (type == "triangle") {
            Vector3 v0 = {shapeData["v0"][0], shapeData["v0"][1], shapeData["v0"][2]};
            Vector3 v1 = {shapeData["v1"][0], shapeData["v1"][1], shapeData["v1"][2]};
            Vector3 v2 = {shapeData["v2"][0], shapeData["v2"][1], shapeData["v2"][2]};
            shapes.push_back(std::make_shared<Triangle>(v0, v1, v2, material));
        }
    }
    return shapes;
}

// Parse lights from JSON data
std::vector<std::shared_ptr<Light>> IO::loadLightsFromJSON(const json& sceneData) {
    std::vector<std::shared_ptr<Light>> lights;

    // Check if the scene has light sources
    if (sceneData.contains("lightsources")) {
        // Loop through the light sources
        for (const auto& lightData : sceneData["lightsources"]) {
            // if (lightData["type"] == "pointlight") {
            //     lights.push_back(std::make_shared<PointLight>(
            //         std::array<float, 3>{lightData["position"][0], lightData["position"][1], lightData["position"][2]},
            //         std::array<float, 3>{lightData["intensity"][0], lightData["intensity"][1], lightData["intensity"][2]}
            //     ));
            // }
            std::string lightType = lightData["type"];
            if (lightType == "pointlight") {
                lights.push_back(std::make_shared<PointLight>(
                    std::array<float, 3>{lightData["position"][0], 
                                       lightData["position"][1], 
                                       lightData["position"][2]},
                    std::array<float, 3>{lightData["intensity"][0], 
                                       lightData["intensity"][1], 
                                       lightData["intensity"][2]}
                ));
            }
            else if (lightType == "arealight") {
                lights.push_back(std::make_shared<AreaLight>(
                    std::array<float, 3>{lightData["position"][0], 
                                       lightData["position"][1], 
                                       lightData["position"][2]},
                    std::array<float, 3>{lightData["intensity"][0], 
                                       lightData["intensity"][1], 
                                       lightData["intensity"][2]},
                    std::array<float, 3>{lightData["u_axis"][0], 
                                       lightData["u_axis"][1], 
                                       lightData["u_axis"][2]},
                    std::array<float, 3>{lightData["v_axis"][0], 
                                       lightData["v_axis"][1], 
                                       lightData["v_axis"][2]}
                ));
            }
        }
    }
    return lights;
}

// Parse a scene from JSON data
Scene IO::loadSceneFromJSON(const json& sceneData) {
    return Scene(
        std::array<float, 3>{sceneData["backgroundcolor"][0], sceneData["backgroundcolor"][1], sceneData["backgroundcolor"][2]},
        loadLightsFromJSON(sceneData),
        loadShapesFromJSON(sceneData)
    );
}

// Parse a render mode from a string
Renderer::RenderMode IO::parseRenderMode(const std::string& renderModeStr) {
    if (renderModeStr == "binary") {
        return Renderer::RenderMode::BINARY;
    } else if (renderModeStr == "phong") {
        return Renderer::RenderMode::PHONG;
    } else if (renderModeStr == "pathtracer") {
        return Renderer::RenderMode::PATHTRACER;
    }
    throw std::invalid_argument("Unknown render mode: " + renderModeStr);
}

// Write the pixel colors to a PPM file
void IO::writePPM(const std::vector<std::vector<Color>>& pixelColors, const std::string& filename) {
    try {
        // Create output directory if it doesn't exist
        std::filesystem::path outputDir = "renders";
        if (!std::filesystem::exists(outputDir)) {
            std::filesystem::create_directories(outputDir);
        }

        // Strip .json and append .ppm
        std::string outputFilename = filename;
        size_t jsonExtPos = outputFilename.rfind(".json");
        if (jsonExtPos != std::string::npos) {
            outputFilename = outputFilename.substr(0, jsonExtPos);
        }
        outputFilename += ".ppm";

        // Open the file for writing
        std::filesystem::path outputPath = outputDir / outputFilename;
        std::ofstream outFile(outputPath);
        
        if (!outFile.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + outputPath.string());
        }

        int height = pixelColors.size();
        int width = pixelColors[0].size();
        
        // Write the PPM header
        outFile << "P3\n";  // Plain text PPM format
        outFile << "# Created by Renderer\n";  // Optional comment line
        outFile << width << " " << height << "\n";  // Image dimensions
        outFile << "255\n";  // Max color value (for 8-bit color depth)

        // Write the pixel data (row by row)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                outFile << pixelColors[y][x].toString() << " ";
            }
            outFile << "\n";  // Newline after each row
        }
        
        outFile.close();
        std::cout << "Image successfully written to: " << outputPath << std::endl;
        
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing PPM file: " << e.what() << std::endl;
    }
}