#include "vectra/physics/forces/newtonian_gravity.h"

#include <iostream>
#include <ostream>



NewtonianGravity::NewtonianGravity(std::vector<GameObject>& game_objects, const linkit::real g_const)
    : affected_objects(game_objects), gravitational_constant(g_const)
{
}

void NewtonianGravity::update_force(GameObject& obj, linkit::real dt)
{
    if (!obj.rb.has_finite_mass()) return;

    for (auto& other : affected_objects)
    {
        if (&other == &obj) continue;
        if (!other.rb.has_finite_mass()) continue;

        linkit::Vector3 obj_to_other = other.rb.transform.position - obj.rb.transform.position;
        linkit::real distance_sq = obj_to_other*obj_to_other;

        // Avoid division by zero or huge forces at close distances
        if (distance_sq < linkit::REAL_EPSILON) continue;

        linkit::Vector3 force_dir = obj_to_other.normalized();

        linkit::real force_magnitude = (gravitational_constant * obj.rb.mass * other.rb.mass) / distance_sq;
        obj.rb.add_force(force_magnitude * force_dir);
    }
}