#include "BVHNode.h"
#include <iostream>
#include <limits>

BVHNode::BVHNode(std::vector<std::shared_ptr<Shape>>& objects, size_t start, size_t end)
    {
     
    // // Randomly choose an axis to split on (0 = x, 1 = y, 2 = z)
    int axis = rand() % 3;
    size_t object_span_size = end - start;

    if (object_span_size == 1) {
        // Leaf node - store the actual shape
        shape = objects[start];
        box = shape->getBoundingBox();
    }
    else if (object_span_size == 2) {
        if (compareBox(objects[start], objects[start+1], axis)) {
            left = std::make_shared<BVHNode>(objects, start, start + 1);
            right = std::make_shared<BVHNode>(objects, start + 1, end);
        } else {
            left = std::make_shared<BVHNode>(objects, start + 1, end);
            right = std::make_shared<BVHNode>(objects, start, start + 1);
        }
        AABB box_left = left->getBoundingBox();
        AABB box_right = right->getBoundingBox();
        box = AABB::surrounding_box(box_left, box_right);
    }
    else {
        // Sort objects based on the chosen axis
        std::sort(objects.begin() + start, objects.begin() + end,
            [axis](const auto& a, const auto& b) {
                return compareBox(a, b, axis);
            });

        auto mid = start + object_span_size/2;
        left = std::make_shared<BVHNode>(objects, start, mid);
        right = std::make_shared<BVHNode>(objects, mid, end);
        
        AABB box_left = left->getBoundingBox();
        AABB box_right = right->getBoundingBox();
        box = AABB::surrounding_box(box_left, box_right);
    }
}

bool BVHNode::intersect(const Ray& ray, float& t, std::shared_ptr<Shape>& hitShape) const {

    if (!box.intersect(ray)) {
        return false;
    }

    // If this is a leaf node with a shape
    if (shape) {
        bool hit = shape->intersect(ray, t);
        if (hit) {
            hitShape = shape;
        }
        return hit;
    }

    std::shared_ptr<Shape> left_shape = nullptr;   // Add these temporary shape holders
    std::shared_ptr<Shape> right_shape = nullptr;

    // Test both children
    float t_left = std::numeric_limits<float>::max();
    float t_right = std::numeric_limits<float>::max();
    
    bool hit_left = left && left->intersect(ray, t_left, left_shape);
    
    bool hit_right = right && right->intersect(ray, t_right, right_shape);

    if (hit_left && hit_right) {
        if (t_left < t_right) {
            t = t_left;
            hitShape = left_shape;    // Use the shape from the closer intersection
        } else {
            t = t_right;
            hitShape = right_shape;
        }
        return true;
    }
    else if (hit_left) {
        t = t_left;
        hitShape = left_shape;
        return true;
    }
    else if (hit_right) {
        t = t_right;
        hitShape = right_shape;
        return true;
    }

    return false;
}

std::shared_ptr<Shape> BVHNode::getShape() const {
    return shape ? shape : nullptr;
}

std::shared_ptr<BVHNode> BVHNode::getLeft() const {
    return left ? left: nullptr;
}

std::shared_ptr<BVHNode> BVHNode::getRight() const {
    return right ? right : nullptr;
}

bool BVHNode::compareBox(const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b, int axis) {
    return a->getBoundingBox().min[axis] < b->getBoundingBox().min[axis];
}

AABB BVHNode::getBoundingBox() const {
    return box;
}