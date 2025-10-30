#ifndef VECTRA_SIMPLE_GRAVITY_H
#define VECTRA_SIMPLE_GRAVITY_H

#include "linkit/linkit.h"
#include "vectra/physics/force_generator.h"

class SimpleGravity : public ForceGenerator
{

    linkit::Vector3 gravitational_field;

public:
    explicit SimpleGravity(linkit::real acceleration = -9.81);
    explicit SimpleGravity(const linkit::Vector3& field = linkit::Vector3(0, -9.81, 0));
    void update_force(GameObject& obj, linkit::real dt) override;
};

#endif //VECTRA_SIMPLE_GRAVITY_H