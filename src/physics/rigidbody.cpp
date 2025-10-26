//
// Created by carlos on 9/29/25.
//

#include "vectra/physics/rigidbody.h"

#include <iostream>
#include <ostream>


Rigidbody::Rigidbody()
{
    transform = Transform();

    velocity = linkit::Vector3(0.0, 0.0f, 0.0f);
    acceleration = linkit::Vector3(0.0f, 0.0f, 0.0f);

    angular_velocity = linkit::Vector3(0.0f, 0.0f, linkit::PI/4);
    angular_acceleration = linkit::Vector3(0.0f, 0.0f, 0.0f);

    mass = 1.0f;
    inverse_mass = (mass > 0.0f) ? (1.0f / mass) : 0.0f; // Handle infinite mass case

    linear_damping = 1.0f;
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
    transform.position += velocity * dt;

}

void Rigidbody::step(const linkit::real dt)
{
    if (mass <= 0.0f) return; // Object is immovable

    step_position(dt);
    step_rotation(dt);
}


