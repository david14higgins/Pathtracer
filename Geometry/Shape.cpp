#include "Shape.h"
#include <sstream>

// Convert the shape to a string
std::string Shape::toString() const {
    std::ostringstream oss;
    oss << "Shape {" << std::endl;
    oss << "  Material: " << material.toString() << std::endl; // Directly access material
    oss << "}";
    return oss.str();
}