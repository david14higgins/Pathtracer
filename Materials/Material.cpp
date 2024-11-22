#include "Material.h"
#include <sstream> // Include for std::ostringstream

// Parameterized constructor
Material::Material(float ks, float kd, float specularexponent, const std::array<float, 3>& diffusecolor,
                   const std::array<float, 3>& specularcolor, bool isreflective, float reflectivity,
                   bool isrefractive, float refractiveindex, bool hasTexture, const std::string& textureFilename)
    : ks(ks), kd(kd), specularexponent(specularexponent), diffusecolor(diffusecolor), specularcolor(specularcolor),
      isreflective(isreflective), reflectivity(reflectivity), isrefractive(isrefractive), refractiveindex(refractiveindex),
      hasTexture(hasTexture), textureFilename(textureFilename) {
        if (hasTexture && !textureFilename.empty()) {
            texture = std::make_shared<Texture>();
            if (!texture->loadFromPPM("textures/" + textureFilename)) {
                std::cerr << "Failed to load texture: " << textureFilename << std::endl;
                hasTexture = false;
            }
        }
    }

std::string Material::toString() const {
    std::ostringstream oss;
    oss << "Material {"
        << "\n  ks: " << ks
        << "\n  kd: " << kd
        << "\n  specularexponent: " << specularexponent
        << "\n  diffusecolor: [" << diffusecolor[0] << ", " << diffusecolor[1] << ", " << diffusecolor[2] << "]"
        << "\n  specularcolor: [" << specularcolor[0] << ", " << specularcolor[1] << ", " << specularcolor[2] << "]"
        << "\n  isreflective: " << (isreflective ? "true" : "false")
        << "\n  reflectivity: " << reflectivity
        << "\n  isrefractive: " << (isrefractive ? "true" : "false")
        << "\n  refractiveindex: " << refractiveindex
        << "\n  hasTexture: " << (hasTexture ? "true" : "false")
        << "\n  textureFilename: " << textureFilename
        << "\n}";
    return oss.str();
}