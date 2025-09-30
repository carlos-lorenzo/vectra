//
// Created by carlos on 9/29/25.
//

#include "vectra/physics/rigidbody.h"


Rigidbody::Rigidbody()
{
    transform = Transform();
    velocity = linkit::Vector3(0.0f, 0.0f, 0.0f);
    acceleration = linkit::Vector3(0.0f, 0.0f, 0.0f);
    mass = 1.0f;
}

void Rigidbody::step(const linkit::real dt)
{
    if (mass <= 0.0f) return; // Object is immovable
    // Update velocity based on acceleration
    velocity += acceleration * dt;
    transform.position += velocity * dt;
}
