#ifndef VECTRA_DEBUG_DRAWER_H
#define VECTRA_DEBUG_DRAWER_H

#include "vectra/core/scene.h"
#include "vectra/rendering/shader.h"
#include "vectra/rendering/model.h"
#include <glm/glm.hpp>

class DebugDrawer {
public:
    DebugDrawer();
    void draw_bvh(BVHNode<BoundingSphere>* root, const glm::mat4& view, const glm::mat4& projection);

private:
    Shader debug_shader_;
    std::unique_ptr<Model> sphere_model_;

    void draw_node(BVHNode<BoundingSphere>* node, const glm::mat4& view, const glm::mat4& projection);
};

#endif //VECTRA_DEBUG_DRAWER_H