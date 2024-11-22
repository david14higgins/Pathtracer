#pragma once

#include <string>
#include <nlohmann/json.hpp>  // Updated include path
#include "../Core/Renderer.h"
#include "../Core/Scene.h"
#include "../Core/Camera.h"
#include "../Materials/Material.h"
#include "../Lighting/Light.h"

using json = nlohmann::json;

class IO {
public:
    static Renderer loadRenderer(const std::string& filename, bool useBVH, bool useAntiAliasing = false, int samplesPerPixel = 4);
    static void writePPM(const std::vector<std::vector<Color>>& pixelColors);

private:
    static Material parseMaterial(const json& materialData);
    static Camera parseCamera(const json& cameraData);
    static std::vector<std::shared_ptr<Shape>> loadShapesFromJSON(const json& sceneData);
    static std::vector<std::shared_ptr<Light>> loadLightsFromJSON(const json& sceneData);
    static Scene loadSceneFromJSON(const json& sceneData);
    static Renderer::RenderMode parseRenderMode(const std::string& renderModeStr);
    static json parseJSONFile(const std::string& filepath);
};