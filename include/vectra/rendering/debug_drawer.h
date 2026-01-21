#ifndef VECTRA_DEBUG_DRAWER_H
#define VECTRA_DEBUG_DRAWER_H

#include "vectra/core/scene.h"
#include "vectra/rendering/shader.h"
#include "vectra/rendering/model.h"
#include "vectra/rendering/camera.h"
#include "vectra/rendering/scene_lights.h"

#include <glm/glm.hpp>

class DebugDrawer {
public:
    DebugDrawer();
    void draw_bvh(BVHNode<BoundingSphere>* root, const glm::mat4& view, const glm::mat4& projection);
    void draw_force(const Transform& object_transform, const linkit::Vector3& force_vector, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& camera_position);
    void draw_spring(const linkit::Vector3& pos_a, const linkit::Vector3& pos_b, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& camera_position);
    void set_light_sources(const SceneLights& scene_lights, glm::vec3 camera_position) const;
private:
    Shader debug_shader_;
    std::unique_ptr<Model> sphere_model_;
    std::unique_ptr<Model> vector_model_;
    std::unique_ptr<Model> spring_model_;

    void draw_node(BVHNode<BoundingSphere>* node, const glm::mat4& view, const glm::mat4& projection);
};

#endif //VECTRA_DEBUG_DRAWER_H