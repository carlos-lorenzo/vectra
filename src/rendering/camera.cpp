#include <iostream>
#include <ostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "vectra/rendering/camera.h"
#include "vectra/rendering/utils.h"


Camera::Camera()
{
    transform = Transform();

    transform.rotation = linkit::Quaternion(0, linkit::Vector3(0, 0, 1));

    transform.position = linkit::Vector3(0.0f, 0.01f, 10.0f);
    fov = 90.0f;
    aspectRatio = 16.0f / 9.0f;
    nearPlane = 0.01f;
    farPlane = 100.0f;

    movement_speed = 2;
    mouse_sensitivity = 0.003;
    max_pitch = linkit::PI * 0.5 - 0.001;

}

glm::mat4 Camera::get_view_matrix()
{
    glm::vec3 pos = glm::vec3(transform.position.x, transform.position.y, transform.position.z);

    glm::vec3 target = pos + vector3_to_vec3(transform.forward());
    glm::vec3 up = vector3_to_vec3(transform.up_dir());
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
    linkit::Vector3 rotation_axis = obj_transform.rotation.axis();
    //std::cout << rotation_axis.to_string() << std::endl;
    model = glm::rotate(model, static_cast<float>(obj_transform.rotation.angle_radians()), glm::vec3(rotation_axis.x, rotation_axis.y, rotation_axis.z));
    // model = glm::rotate(model, glm::radians(static_cast<float>(obj_transform.rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::rotate(model, glm::radians(static_cast<float>(obj_transform.rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f));

    // Apply scale
    model = glm::scale(model, glm::vec3(obj_transform.scale.x, obj_transform.scale.y, obj_transform.scale.z));
    return model;
}

glm::mat4 Camera::get_model_matrix(const Transform& obj_transform)
{
    // 2. Generate the model matrix
    // Start with an identity matrix
    glm::mat4 model = glm::mat4(1.0f);
    // Apply transformations: Translate, then Rotate, then Scale (applied in reverse order)
    model = glm::translate(model, glm::vec3(obj_transform.position.x, obj_transform.position.y, obj_transform.position.z));
    // Apply rotations for each axis
    linkit::Vector3 rotation_axis = obj_transform.rotation.axis();
    //std::cout << rotation_axis.to_string() << std::endl;
    model = glm::rotate(model, static_cast<float>(obj_transform.rotation.angle_radians()), glm::vec3(rotation_axis.x, rotation_axis.y, rotation_axis.z));
    // model = glm::rotate(model, glm::radians(static_cast<float>(obj_transform.rotation.y)), glm::vec3(0.0f, 1.0f, 0.0f));
    // model = glm::rotate(model, glm::radians(static_cast<float>(obj_transform.rotation.z)), glm::vec3(0.0f, 0.0f, 1.0f));

    // Apply scale
    model = glm::scale(model, glm::vec3(obj_transform.scale.x, obj_transform.scale.y, obj_transform.scale.z));
    return model;
}


