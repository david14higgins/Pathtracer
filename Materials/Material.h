#ifndef MATERIAL_H
#define MATERIAL_H

#include <array>
#include <string>
#include "Texture.h"
#include <memory>

class Material {
public:
    // Default constructor with default values for all member variables
    Material()
        : ks(0.0f), kd(0.0f), specularexponent(0.0f),
          diffusecolor{0.0f, 0.0f, 0.0f}, specularcolor{0.0f, 0.0f, 0.0f},
          isreflective(false), reflectivity(0.0f), isrefractive(false), refractiveindex(1.0f), hasTexture(false), textureFilename("") {}

    // Parameterized constructor
    Material(float ks, float kd, float specularexponent, const std::array<float, 3>& diffusecolor,
             const std::array<float, 3>& specularcolor, bool isreflective, float reflectivity,
             bool isrefractive, float refractiveindex, bool hasTexture = false, const std::string& textureFilename = "");

    std::string toString() const;

    std::shared_ptr<Texture> getTexture() const { return texture; }

    float ks, kd, specularexponent; // Specular and diffuse coefficients
    std::array<float, 3> diffusecolor; // Diffuse color
    std::array<float, 3> specularcolor; // Specular color
    bool isreflective; // Reflective flag
    float reflectivity; // Reflectivity coefficient
    bool isrefractive; // Refractive flag
    float refractiveindex; // Refractive index
    bool hasTexture; // Texture flag
    std::string textureFilename; // Texture filename

private:
    std::shared_ptr<Texture> texture; // Texture pointer
};

#endif