//
// Created by carlos on 9/29/25.
//

#include "vectra/physics/transform.h"

#include "linkit/quaternion.h"

void Transform::translate(const linkit::Vector3& delta)
{
    position = position + delta;
}

Transform::Transform(const linkit::Vector3& init_position, const linkit::Quaternion& init_rotation, const linkit::Vector3& init_scale)
{
    position = init_position;
    rotation = init_rotation;
    scale = init_scale;
}
Transform::Transform()
{
    position = linkit::Vector3(0.0f, 0.0f, 0.0f);
    rotation = linkit::Quaternion(2*linkit::PI, linkit::Vector3(0.0f, 0.0f, 1.0f));
    scale = linkit::Vector3(1.0f, 1.0f, 1.0f);
}


linkit::Vector3 Transform::forward()
{
    return -1 * rotation.axis();
}


