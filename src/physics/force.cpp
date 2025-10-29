#include "vectra/physics/force.h"

Force::Force()
{
    initialize();
}

void Force::initialize()
{
    accumulated_force = linkit::Vector3(0, 0, 0);
    accumulated_torque = linkit::Vector3(0, 0, 0);
}


void Force::compute_force(GameObject affected_particle, std::vector<GameObject> game_objects)
{
    accumulated_force = linkit::Vector3(0, 0, 0);
}

void Force::compute_torque(GameObject affected_particle, std::vector<GameObject> game_objects)
{
    accumulated_torque = linkit::Vector3(0, 0, 0);
}

void Force::apply_effect(GameObject affected_particle, std::vector<GameObject> game_objects)
{
    compute_force(affected_particle, game_objects);
    compute_torque(affected_particle, game_objects);
    affected_particle.rb.accumulated_force += accumulated_force;
    affected_particle.rb.accumulated_torque += accumulated_torque;

    initialize();

}


