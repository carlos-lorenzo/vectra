#include "vectra/rendering/camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Camera::Camera()
{
    transform = Transform();
    transform.position = linkit::Vector3(0.0f, 0.0f, 10.0f);
    fov = 45.0f;
    aspectRatio = 4.0f / 3.0f;
    nearPlane = 0.1f;
    farPlane = 100.0f;
}

void Camera::transform_object(GameObject& obj)
{

}

glm::mat4 Camera::get_view_matrix() const
{
    glm::vec3 pos = glm::vec3(transform.position.x, transform.position.y, transform.position.z);
    glm::vec3 target = pos + glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    return glm::lookAt(pos, target, up);
}


glm::mat4 Camera::get_projection_matrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}


glm::mat4 Camera::get_model_matrix(const GameObject &obj)
{
    // 1. Get the transform from the game object
    const Transform& obj_transform = obj.rb.transform;

    // 2. Generate the model matrix
    // Start with an identity matrix
    glm::mat4 model = glm::mat4(1.0f);
    // Apply transformations: Translate, then Rotate, then Scale (applied in reverse order)
    model = glm::translate(model, glm::vec3(obj_transform.position.x, obj_transform.position.y, obj_transform.position.z));
    // Apply rotations for each axis
    model = glm::rotate(model, glm::radians(obj_transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj_transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(obj_transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    // Apply scale
    model = glm::scale(model, glm::vec3(obj_transform.scale.x, obj_transform.scale.y, obj_transform.scale.z));
    return model;
}
