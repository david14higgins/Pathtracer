#include "Ray.h"
#include <sstream>

std::string Ray::toString() const {
    std::ostringstream oss;
    oss << "Ray {\n"
        << "  Origin: " << origin.toString() << "\n"
        << "  Direction: " << direction.toString() << "\n"
        << "}";
    return oss.str();
}