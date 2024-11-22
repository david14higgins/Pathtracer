#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "Shape.h"
#include "AABB.h"
#include <memory>
#include <vector>
#include <algorithm>

class BVHNode {
public:
    BVHNode(std::vector<std::shared_ptr<Shape>>& objects, size_t start, size_t end);

    bool intersect(const Ray& ray, float& t, std::shared_ptr<Shape>& hitShape) const;
    AABB getBoundingBox() const;

    std::shared_ptr<Shape> getShape() const;
    std::shared_ptr<BVHNode> getLeft() const;
    std::shared_ptr<BVHNode> getRight() const;

private:
    std::shared_ptr<BVHNode> left;
    std::shared_ptr<BVHNode> right;
    std::shared_ptr<Shape> shape;  // For leaf nodes
    AABB box;

    static bool compareBox(const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b, int axis);
};

#endif