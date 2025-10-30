#include "vectra/physics/forces/simple_gravity.h"

#include <iostream>
#include <ostream>

SimpleGravity::SimpleGravity(const linkit::real acceleration)
{
    gravitational_field = linkit::Vector3(0, acceleration, 0);
}

SimpleGravity::SimpleGravity(const linkit::Vector3& field) : gravitational_field(field) {};

void SimpleGravity::update_force(GameObject& obj, linkit::real dt)
{
    obj.rb.accumulated_force += obj.rb.mass * gravitational_field;
}
