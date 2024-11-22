#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>
#include <array>
#include <sstream>
class Vector3 {
public:
    // Constructors
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Accessors
    float length() const { return std::sqrt(x*x + y*y + z*z); }
    
    Vector3 normalize() const {
        float len = length();
        return (len > 0) ? *this / len : *this;
    }

    // Operator overloads
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Method to convert std::array<float, 3> to Vector3
    static Vector3 fromArray(const std::array<float, 3>& arr) {
        return Vector3(arr[0], arr[1], arr[2]);
    }

    // For debugging (printing the vector)
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    // Add toString method
    std::string toString() const {
        std::ostringstream oss;
        oss << "[" << x << ", " << y << ", " << z << "]";
        return oss.str();
    }

        // Array-style accessor
    float operator[](int i) const {
        switch(i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Vector3 index out of range");
        }
    }

    // Array-style modifier
    float& operator[](int i) {
        switch(i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range("Vector3 index out of range");
        }
    }

public:
    float x, y, z;
};

#endif // VECTOR3_H