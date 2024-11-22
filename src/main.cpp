#include <iostream>
#include <chrono>
#include "../IO/IO.h"
#include "../Core/Renderer.h"
#include "../Materials/Color.h"


int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            std::cout << "Usage: " << argv[0] << " <scene_file.json>" << std::endl;
            std::cout << "Please provide a path to a scene file." << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --bvh    Enable BVH acceleration structure" << std::endl;
            return 0;
        }

        std::string filename = argv[1];

        bool useBVH = false;
        for (int i = 2; i < argc; i++) {
            if (std::string(argv[i]) == "--bvh") {
                useBVH = true;
                break;
            }
        }
        std::cout << (useBVH ? "BVH acceleration enabled" : "BVH acceleration disabled") << std::endl;

        bool useAntiAliasing = true;
        int samplesPerPixel = 16;

        Renderer renderer = IO::loadRenderer(filename, useBVH, useAntiAliasing, samplesPerPixel);
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<Color>> renderedScene = renderer.renderScene();
        IO::writePPM(renderedScene);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Render time: " << duration.count() << " milliseconds" << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in main: " << e.what() << std::endl;
        return 1;
    }
}