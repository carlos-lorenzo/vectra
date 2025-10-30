#ifndef VECTRA_NEWTONIAN_GRAVITY_H
#define VECTRA_NEWTONIAN_GRAVITY_H


#include "linkit/linkit.h"
#include "vectra/physics/force_generator.h"

class NewtonianGravity : public ForceGenerator
{

    linkit::real gravitational_constant;

public:
    explicit NewtonianGravity(linkit::real g_const = 6.67e-11);
    void update_force(GameObject& obj, const std::vector<GameObject>& all_objects, linkit::real dt) override;
};


#endif //VECTRA_NEWTONIAN_GRAVITY_H