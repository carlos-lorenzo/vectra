#ifndef VECTRA_FORCE_GENERATOR_H
#define VECTRA_FORCE_GENERATOR_H

#include <vector>
#include "vectra/core/gameobject.h"

class ForceGenerator
{
public:
    virtual ~ForceGenerator() = default;
    /**
     * Calculates and applies the force to the given GameObject.
     */
    virtual void update_force(GameObject& obj, const std::vector<GameObject>& all_objects, linkit::real dt) = 0;
};

#endif //VECTRA_FORCE_GENERATOR_H