//
// Created by carlos on 9/29/25.
//

#include "vectra/physics/rigidbody.h"

#include <iostream>
#include <ostream>


Rigidbody::Rigidbody()
{
    transform = Transform();

    velocity = linkit::Vector3(0.0, 0.0, 0.0);
    acceleration = linkit::Vector3(0.0f, 0.0f, 0.0f);

    angular_velocity = linkit::Vector3(0.0f, 0.0f, 0.0f);
    angular_acceleration = linkit::Vector3(0.0f, 0.0f, 0.0f);

    accumulated_force = linkit::Vector3(0.0f, 0.0f, 0.0f);
    accumulated_torque = linkit::Vector3(0.0f, 0.0f, 0.0f);

    // Will be set by child? constructor? bounding box?
    _local_inverse_inertia_tensor = linkit::Matrix3();

    mass = 1.0f;
    inverse_mass = (mass > 0.0f) ? (1.0f / mass) : 0.0f; // Handle infinite mass case

    linear_damping = 1.0f;
}

void Rigidbody::clear_accumulators()
{
    accumulated_force = linkit::Vector3(0.0f, 0.0f, 0.0f);
    accumulated_torque = linkit::Vector3(0.0f, 0.0f, 0.0f);
}

linkit::Matrix4 Rigidbody::transform_matrix()
{
    return linkit::Matrix4::object_transform_matrix(transform.position, transform.rotation, transform.scale);
}

linkit::Matrix4 Rigidbody::inverse_transform_matrix()
{
    return linkit::Matrix4::inverse_object_transform_matrix(transform.position, transform.rotation, transform.scale);
}


linkit::Vector3 Rigidbody::local_to_world(const linkit::Vector3& local_point)
{
    return transform_matrix() * local_point;
}

linkit::Vector3 Rigidbody::world_to_local(const linkit::Vector3& world_point)
{
    return inverse_transform_matrix() * world_point;
}


linkit::Matrix3 Rigidbody::get_inverse_inertia_tensor() const
{

    // Change of basis optimisation as the inverse of a rotation matrix is its transpose
    return transform.rotation.to_matrix3() * _local_inverse_inertia_tensor * transform.rotation.to_matrix3().transposed();
}

void Rigidbody::add_force(const linkit::Vector3& force)
{
    accumulated_force += force;
}

void Rigidbody::add_force_at_world_point(const linkit::Vector3& force, const linkit::Vector3& point)
{
    accumulated_force += force;
    linkit::Vector3 lever_arm = point - transform.position;
    accumulated_torque += lever_arm % force;
}

void Rigidbody::add_force_at_local_point(const linkit::Vector3& force, const linkit::Vector3& point)
{
    add_force_at_world_point(force, local_to_world(point));
}

void Rigidbody::compute_accelerations()
{
    acceleration = accumulated_force * inverse_mass;
    angular_acceleration = get_inverse_inertia_tensor() * accumulated_torque;
    std::cout << get_inverse_inertia_tensor().to_string() << std::endl;

}


void Rigidbody::step_rotation(const linkit::real dt)
{

    angular_velocity += angular_acceleration * dt;
    transform.rotation.add_scaled_vector(angular_velocity, dt);
    transform.rotation.normalize();
}

void Rigidbody::step_position(const linkit::real dt)
{
    velocity += acceleration * dt;
    //velocity *= linkit::real_pow(linear_damping, dt);
    transform.position += velocity * dt;
}

void Rigidbody::step(const linkit::real dt)
{
    if (!has_finite_mass()) return; // Object is immovable

    compute_accelerations();
    step_position(dt);
    step_rotation(dt);
}

bool Rigidbody::has_finite_mass() const
{
    return inverse_mass != 0;
}

linkit::Matrix3 Rigidbody::cuboid_inertia_tensor() const
{
    linkit::Matrix3 inertia_tensor;
    inertia_tensor.m[0][0] = (1.0f / 12.0f) * mass * (transform.scale.y * transform.scale.y + transform.scale.z * transform.scale.z);
    inertia_tensor.m[1][1] = (1.0f / 12.0f) * mass * (transform.scale.x * transform.scale.x + transform.scale.z * transform.scale.z);
    inertia_tensor.m[2][2] = (1.0f / 12.0f) * mass * (transform.scale.x * transform.scale.x + transform.scale.y * transform.scale.y);
    return inertia_tensor;
}


void Rigidbody::set_inverse_inertia_tensor(const linkit::Matrix3& inertia_tensor)
{
    _local_inverse_inertia_tensor = inertia_tensor.inverse();
}
