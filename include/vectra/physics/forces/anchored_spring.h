#ifndef VECTRA_ANCHORED_SPRING_H
#define VECTRA_ANCHORED_SPRING_H

#include "linkit/linkit.h"
#include "vectra/physics/force_generator.h"


class AnchoredSpring : public ForceGenerator
{
protected:
    linkit::Vector3 anchor_point;
    linkit::real spring_constant;
    linkit::real rest_length;
    linkit::real damping;

public:
    explicit AnchoredSpring(const linkit::Vector3& anchor_point, linkit::real spring_constant, linkit::real rest_length, linkit::real damping);
    void update_force(GameObject& obj, linkit::real dt) override;
};

#endif //VECTRA_ANCHORED_SPRING_H