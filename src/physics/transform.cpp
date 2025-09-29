//
// Created by carlos on 9/29/25.
//

#include "vectra/physics/transform.h"

void Transform::translate(const linkit::Vector3& delta)
{
    position = position + delta;
}

Transform::Transform(linkit::Vector3 init_position, linkit::Vector3 init_rotation, linkit::Vector3 init_scale)
{
    position = init_position;
    rotation = init_rotation;
    scale = init_scale;
}
Transform::Transform()
{
    position = linkit::Vector3(0.0f, 0.0f, 0.0f);
    rotation = linkit::Vector3(0.0f, 0.0f, 0.0f);
    scale = linkit::Vector3(1.0f, 1.0f, 1.0f);
}

