#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "../Geometry/Shape.h"
#include "AABB.h"
#include <memory>
#include <vector>
#include <algorithm>

class BVHNode {
public:
    BVHNode(std::vector<std::shared_ptr<Shape>>& objects, size_t start, size_t end); // Constructor for BVHNode

    bool intersect(const Ray& ray, float& t, std::shared_ptr<Shape>& hitShape) const; // Intersection test with a ray
    AABB getBoundingBox() const; // Get the bounding box of the node

    std::shared_ptr<Shape> getShape() const; // Get the shape of the node
    std::shared_ptr<BVHNode> getLeft() const; // Get the left child of the node
    std::shared_ptr<BVHNode> getRight() const; // Get the right child of the node

private:
    std::shared_ptr<BVHNode> left; // Left child of the node
    std::shared_ptr<BVHNode> right; // Right child of the node
    std::shared_ptr<Shape> shape;  // For leaf nodes
    AABB box; // Bounding box of the node

    static bool compareBox(const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b, int axis); // Compare bounding boxes along an axis
};

#endif