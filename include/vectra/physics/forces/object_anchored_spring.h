#ifndef VECTRA_OBJECT_ANCHORED_SPRING_H
#define VECTRA_OBJECT_ANCHORED_SPRING_H

#include "linkit/linkit.h"

#include "vectra/core/gameobject.h"
#include "vectra/physics/force_generator.h"


class ObjectAnchoredSpring : public ForceGenerator
{
protected:

public:
    GameObject* anchor_object;
    linkit::real spring_constant;
    linkit::real rest_length;
    linkit::real damping;

    explicit ObjectAnchoredSpring(GameObject* anchor_object, linkit::real spring_constant, linkit::real rest_length, linkit::real damping);
    void update_force(GameObject& obj, linkit::real dt) override;
    [[nodiscard]] linkit::Vector3 get_anchor_point() const;
};

#endif //VECTRA_OBJECT_ANCHORED_SPRING_H