#ifndef VECTRA_FORCE_H
#define VECTRA_FORCE_H

#include <vector>

#include "vectra/core/gameobject.h"
#include "linkit/linkit.h"

class Force
{
    public:
        linkit::Vector3 accumulated_force;
        linkit::Vector3 accumulated_torque;
    Force();
    void initialize();
    void compute_force(GameObject affected_particle, std::vector<GameObject> game_objects);
    void compute_torque(GameObject affected_particle, std::vector<GameObject> game_objects);
    void apply_effect(GameObject affected_particle, std::vector<GameObject> game_objects);

};

#endif //VECTRA_FORCE_H