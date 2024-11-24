#pragma once

#include <string>
#include <nlohmann/json.hpp> 
#include "../Core/Renderer.h"
#include "../Core/Scene.h"
#include "../Core/Camera.h"
#include "../Materials/Material.h"
#include "../Lighting/Light.h"

using json = nlohmann::json;

class IO {
public:
    // Load the renderer from a JSON file
    static Renderer loadRenderer(const std::string& filename, bool useBVH, bool useAntiAliasing = false, int samplesPerPixel = 4);
    // Write the scene's pixel colors to a PPM file
    static void writePPM(const std::vector<std::vector<Color>>& pixelColors, const std::string& filename);

private:
    static Material parseMaterial(const json& materialData); // Parse a material from JSON data
    static Camera parseCamera(const json& cameraData); // Parse a camera from JSON data
    static std::vector<std::shared_ptr<Shape>> loadShapesFromJSON(const json& sceneData); // Parse shapes from JSON data
    static std::vector<std::shared_ptr<Light>> loadLightsFromJSON(const json& sceneData); // Parse lights from JSON data
    static Scene loadSceneFromJSON(const json& sceneData); // Parse a scene from JSON data
    static Renderer::RenderMode parseRenderMode(const std::string& renderModeStr); // Parse a render mode from a string
    static json parseJSONFile(const std::string& filepath); // Parse a JSON file
};