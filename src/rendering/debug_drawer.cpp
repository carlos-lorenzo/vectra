#include "vectra/rendering/debug_drawer.h"
#include "vectra/rendering/utils.h"
#include "vectra/rendering/camera.h"
#include <glm/gtc/matrix_transform.hpp>

DebugDrawer::DebugDrawer()
    : debug_shader_("resources/shaders/model.vert", "resources/shaders/debug.frag"),
      sphere_model_(std::make_unique<Model>("resources/models/primitives/sphere.obj")) {}

void DebugDrawer::draw_bvh(BVHNode<BoundingSphere>* root, const glm::mat4& view, const glm::mat4& projection) {
    if (!root) return;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    debug_shader_.use();
    draw_node(root, view, projection);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Restore fill mode
}

void DebugDrawer::draw_node(BVHNode<BoundingSphere>* node, const glm::mat4& view, const glm::mat4& projection) {
    if (!node || !node->bounding_volume) return;

    // Set color based on node depth or type
    glm::vec3 color = node->is_leaf() ? glm::vec3(0.0, 1.0, 0.0) : glm::vec3(1.0, 1.0, 0.0);
    debug_shader_.set_vec3("color", color);

    // Calculate model matrix for the sphere
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, vector3_to_vec3(node->bounding_volume->center));
    model = glm::scale(model, glm::vec3(node->bounding_volume->radius));
    glm::mat4 param_view = view;
    glm::mat4 param_projection = projection;
    debug_shader_.set_mat4("model", model);
    debug_shader_.set_mat4("view", param_view);
    debug_shader_.set_mat4("projection", param_projection);

    sphere_model_->draw(debug_shader_);

    if (!node->is_leaf()) {
        draw_node(node->children[0], view, projection);
        draw_node(node->children[1], view, projection);
    }
}