#ifndef VECTOR3_H
#define VECTOR3_H

#include <cmath>
#include <iostream>
#include <array>
#include <sstream>

class Vector3 {
public:
    // Constructors
    Vector3();
    Vector3(float x, float y, float z);

    // Accessors
    float length() const;
    Vector3 normalize() const;

    // Operator overloads
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;

    float dot(const Vector3& other) const;
    Vector3 cross(const Vector3& other) const;

    // Method to convert std::array<float, 3> to Vector3
    static Vector3 fromArray(const std::array<float, 3>& arr);

    // For debugging (printing the vector)
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v);

    // Add toString method
    std::string toString() const;

    // Array-style accessor
    float operator[](int i) const;

    // Array-style modifier
    float& operator[](int i);

public:
    float x, y, z;
};

#endif // VECTOR3_H