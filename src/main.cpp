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
            std::cout << "  --useAA  Enable anti-aliasing" << std::endl;
            return 0;
        }

        std::string filename = argv[1];

        bool useBVH = false;
        bool useAntiAliasing = false;
        int samplesPerPixel = 8;

        for (int i = 2; i < argc; i++) {
            if (std::string(argv[i]) == "--useBVH") {
                useBVH = true;
            }
            if (std::string(argv[i]) == "--useAA") {
                useAntiAliasing = true;
            }
        }
        std::cout << (useBVH ? "BVH acceleration enabled" : "BVH acceleration disabled") << std::endl;
        std::cout << (useAntiAliasing ? "Anti-aliasing enabled" : "Anti-aliasing disabled") << std::endl;
        
        Renderer renderer = IO::loadRenderer(filename, useBVH, useAntiAliasing, samplesPerPixel);
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<std::vector<Color>> renderedScene = renderer.renderScene();
        IO::writePPM(renderedScene, filename);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Render time: " << duration.count() << " milliseconds" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in main: " << e.what() << std::endl;
        return 1;
    }
}