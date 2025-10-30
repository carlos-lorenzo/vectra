#include "vectra/physics/forces/newtonian_gravity.h"

#include <iostream>
#include <ostream>

NewtonianGravity::NewtonianGravity(const linkit::real g_const) : gravitational_constant(g_const) {}

void NewtonianGravity::update_force(GameObject& obj, const std::vector<GameObject>& all_objects, linkit::real dt)
{
    if (!obj.rb.has_finite_mass()) return;
    for (auto& other : all_objects)
    {
        if (&other == &obj) continue;
        if (!other.rb.has_finite_mass()) continue;

        linkit::Vector3 obj_to_other = other.rb.transform.position - obj.rb.transform.position;
        linkit::real distance = obj_to_other.magnitude();
        obj.rb.accumulated_force += ((gravitational_constant * obj.rb.mass * other.rb.mass) / (distance*distance)) * obj_to_other.normalized();


    }
}

