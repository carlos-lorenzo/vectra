#ifndef VECTRA_NEWTONIAN_GRAVITY_H
#define VECTRA_NEWTONIAN_GRAVITY_H


#include "linkit/linkit.h"
#include "vectra/physics/force_generator.h"

class NewtonianGravity : public ForceGenerator
{

    std::vector<GameObject>& affected_objects;
    linkit::real gravitational_constant;

public:
    explicit NewtonianGravity(std::vector<GameObject>& game_objects, linkit::real g_const = 6.67e-11);
    void update_force(GameObject& obj, linkit::real dt) override;
};


#endif //VECTRA_NEWTONIAN_GRAVITY_H