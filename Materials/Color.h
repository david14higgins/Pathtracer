#pragma once
#include <string>
#include "../Geometry/Vector3.h"
#include <algorithm> // For std::clamp

class Color {
private:
    Vector3 rgb; // RGB values as a Vector3

public:
    // Constructor
    Color(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0)
        : rgb(red, green, blue) {}

    // Getters for each color component
    unsigned char getRed() const { return static_cast<unsigned char>(rgb.x); }
    unsigned char getGreen() const { return static_cast<unsigned char>(rgb.y); }
    unsigned char getBlue() const { return static_cast<unsigned char>(rgb.z); }

    // Setters for each color component
    void setRed(unsigned char red) { rgb.x = red; }
    void setGreen(unsigned char green) { rgb.y = green; }
    void setBlue(unsigned char blue) { rgb.z = blue; }

    // Method to return a color as a string in the format "(R, G, B)"
    std::string toString() const {
        return std::to_string(getRed()) + " " + std::to_string(getGreen()) + " " + std::to_string(getBlue());
    }

    // Overload operator for comparing two colors
    bool operator==(const Color &other) const {
        return rgb.x == other.rgb.x && rgb.y == other.rgb.y && rgb.z == other.rgb.z;
    }

    // Overload operator += for adding a Color to the Color
    Color& operator+=(const Color& other) {
        rgb.x += other.rgb.x;
        rgb.y += other.rgb.y;
        rgb.z += other.rgb.z;
        return *this;
    }

    // Overload operator + for adding two Colors
    Color operator+(const Color& other) const {
        return Color(
            static_cast<unsigned char>(std::clamp(rgb.x + other.rgb.x, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.y + other.rgb.y, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.z + other.rgb.z, 0.0f, 255.0f))
        );
    }

    // Overload operator * for multiplying two Colors component-wise
    Color operator*(const Color& other) const {
        return Color(
            static_cast<unsigned char>(std::clamp(rgb.x * other.rgb.x / 255.0f, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.y * other.rgb.y / 255.0f, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.z * other.rgb.z / 255.0f, 0.0f, 255.0f))
        );
    }

    // Overload operator * for scalar multiplication
    Color operator*(float scalar) const {
        return Color(
            static_cast<unsigned char>(std::clamp(rgb.x * scalar, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.y * scalar, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.z * scalar, 0.0f, 255.0f))
        );
    }

    // Clamping method to ensure color values remain in valid range 0-255
    Color clamp() const {
        return Color(
            static_cast<unsigned char>(std::clamp(rgb.x, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.y, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(rgb.z, 0.0f, 255.0f))
        );
    }

    // Method to create a color from an array of floats
    static Color fromFloatArray(const std::array<float, 3>& arr) {
        return Color(
            static_cast<unsigned char>(std::clamp(arr[0] * 255.0f, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(arr[1] * 255.0f, 0.0f, 255.0f)),
            static_cast<unsigned char>(std::clamp(arr[2] * 255.0f, 0.0f, 255.0f))
        );
    }
};