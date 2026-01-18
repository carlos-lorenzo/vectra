#include "vectra/rendering/debug_drawer.h"

#include <unistd.h>

#include "vectra/rendering/utils.h"
#include "vectra/rendering/camera.h"
#include <glm/gtc/matrix_transform.hpp>

DebugDrawer::DebugDrawer()
    : debug_shader_("resources/shaders/model.vert", "resources/shaders/phong.frag"),
      sphere_model_(std::make_unique<Model>("resources/models/primitives/sphere.obj")),
      vector_model_(std::make_unique<Model>("resources/models/debugging/vector.obj")),
      spring_model_(std::make_unique<Model>("resources/models/debugging/spring.obj"))
{}

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





void DebugDrawer::draw_force(const Transform& object_transform, const linkit::Vector3& force_vector, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& camera_position)
{
    constexpr float EPSILON = 1e-6f;
    constexpr float max_length = 5.0f;

    linkit::real force_magnitude = force_vector.magnitude() * 0.2f; // Scale down for visibility
    if (force_magnitude < EPSILON) {
        return; // Nothing to draw
    } if (force_magnitude > max_length) {
        force_magnitude = max_length; // Clamp to max length
    }

    // Arrow model is assumed to point along local +Y.
    linkit::Vector3 model_forward = linkit::Vector3(0.0f, 1.0f, 0.0f);
    linkit::Vector3 desired_direction = force_vector.normalized();

    // Compute rotation from model_forward -> desired_direction
    linkit::real dot_prod = model_forward*desired_direction;
    // Clamp to [-1, 1] to avoid NaNs from arccos
    if (dot_prod > 1.0f) dot_prod = 1.0f;
    if (dot_prod < -1.0f) dot_prod = -1.0f;


    linkit::Quaternion rotation_quat;
    if (dot_prod > 1.0f - 1e-6f) {
        // Vectors are the same -> no rotation
        rotation_quat = linkit::Quaternion(0.0, linkit::Vector3(0.0f, 1.0f, 0.0f));
    } else if (dot_prod < -1.0f + 1e-6f) {
        // Vectors are opposite -> rotate 180 degrees around any perpendicular axis
        linkit::Vector3 perp_axis = linkit::Vector3(1.0f, 0.0f, 0.0f);
        if (std::abs((model_forward%perp_axis).magnitude()) < EPSILON) {
            perp_axis = linkit::Vector3(0.0f, 0.0f, 1.0f);
        }
        rotation_quat = linkit::Quaternion(linkit::PI, perp_axis.normalized());
    } else {
        linkit::Vector3 rotation_axis = model_forward % desired_direction;
        linkit::real axis_length = rotation_axis.magnitude();
        if (axis_length < EPSILON) {
            // Fallback if cross produced near-zero axis
            rotation_quat = linkit::Quaternion(0.0, linkit::Vector3(0.0f, 1.0f, 0.0f));
        } else {
            rotation_axis = rotation_axis / axis_length;
            linkit::real rotation_angle = std::acos(dot_prod);
            rotation_quat = linkit::Quaternion(rotation_angle, rotation_axis);
        }
    }

    // Position the arrow so its base is at the object position:
    // move by half the scaled length along the desired direction
    linkit::Vector3 arrow_translation = object_transform.position + desired_direction * (force_magnitude * 0.5f);

    // Scale the arrow: Y axis represents length
    linkit::Vector3 arrow_scale = linkit::Vector3(1.0f, static_cast<float>(force_magnitude), 1.0f);

    Transform vector_transform = Transform(arrow_translation, rotation_quat, arrow_scale);
    glm::mat4 model_mat = Camera::get_model_matrix(vector_transform);

    glm::mat4 param_view = view;
    glm::mat4 param_projection = projection;
    glm::vec3 param_camera_pos = camera_position;

    debug_shader_.use();
    debug_shader_.set_mat4("model", model_mat);
    debug_shader_.set_mat4("view", param_view);
    debug_shader_.set_mat4("projection", param_projection);
    debug_shader_.set_vec3("camera_position", param_camera_pos);

    vector_model_->draw(debug_shader_);
}

void DebugDrawer::draw_spring(const linkit::Vector3& pos_a, const linkit::Vector3& pos_b, const glm::mat4& view,
    const glm::mat4& projection, const glm::vec3& camera_position)
{

    linkit::Vector3 displacement = pos_b - pos_a;
    linkit::real length = displacement.magnitude();
    if (length < 1e-6f) {
        return; // Nothing to draw
    }
    linkit::Vector3 direction = displacement.normalized();
    // Arrow model scales forward along the local + X axis
    linkit::Vector3 scale = linkit::Vector3(length, 1.0f, 1.0f);

    // Compute rotation from model_forward -> desired_direction
    linkit::Vector3 model_forward = linkit::Vector3(1.0f, 0.0f, 0.0f);
    linkit::Vector3 desired_direction = direction;
    constexpr float EPSILON = 1e-6f;
    linkit::real dot_prod = model_forward * desired_direction;
    // Clamp to [-1, 1] to avoid NaNs from arccos
    if (dot_prod > 1.0f) dot_prod = 1.0f;
    if (dot_prod < -1.0f) dot_prod = -1.0f;


    linkit::Quaternion rotation_quat;
    if (dot_prod > 1.0f - 1e-6f) {
        // Vectors are the same -> no rotation
        rotation_quat = linkit::Quaternion(0.0, linkit::Vector3(0.0f, 1.0f, 0.0f));
    } else if (dot_prod < -1.0f + 1e-6f) {
        // Vectors are opposite -> rotate 180 degrees around any perpendicular axis
        linkit::Vector3 perp_axis = linkit::Vector3(0.0f, 1.0f, 0.0f);
        if (std::abs((model_forward % perp_axis).magnitude()) < EPSILON) {
            perp_axis = linkit::Vector3(0.0f, 0.0f, 1.0f);
        }
        rotation_quat = linkit::Quaternion(linkit::PI, perp_axis.normalized());
    } else {
        linkit::Vector3 rotation_axis = model_forward % desired_direction;
        linkit::real axis_length = rotation_axis.magnitude();
        if (axis_length < EPSILON) {
            // Fallback if cross produced near-zero axis
            rotation_quat = linkit::Quaternion(0.0, linkit::Vector3(0.0f, 1.0f, 0.0f));
        } else {
            rotation_axis = rotation_axis / axis_length;
            linkit::real rotation_angle = std::acos(dot_prod);
            rotation_quat = linkit::Quaternion(rotation_angle, rotation_axis);
        }
    }

    // Spring origin at one end
    Transform spring_transform = Transform(pos_a, rotation_quat, scale);
    glm::mat4 model_mat = Camera::get_model_matrix(spring_transform);

    glm::mat4 param_view = view;
    glm::mat4 param_projection = projection;
    glm::vec3 param_camera_pos = camera_position;

    debug_shader_.use();
    debug_shader_.set_mat4("model", model_mat);
    debug_shader_.set_mat4("view", param_view);
    debug_shader_.set_mat4("projection", param_projection);
    debug_shader_.set_vec3("camera_position", param_camera_pos);

    spring_model_->draw(debug_shader_);
}

void DebugDrawer::set_light_sources(const std::vector<LightSource>& light_sources) const
{
    debug_shader_.use();
    auto light_position = glm::vec3(0.0f);
    auto light_colour = glm::vec3(1.0f);
    if (!light_sources.empty())
    {
        light_position = light_sources[0].position;
        light_colour = light_sources[0].colour;
    }

    debug_shader_.set_vec3("light_  position", light_position);
    debug_shader_.set_vec3("light_colour", light_colour);

}
