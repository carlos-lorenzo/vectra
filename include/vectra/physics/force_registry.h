#ifndef VECTRA_FORCE_REGISTRY_H
#define VECTRA_FORCE_REGISTRY_H

#include <vector>
#include <memory>
#include "vectra/physics/force_generator.h"
#include "vectra/core/gameobject.h"

class ForceRegistry
{
protected:
    struct ForceRegistration
    {
        GameObject* obj;
        std::shared_ptr<ForceGenerator> force_generator;
    };


public:
    std::vector<ForceRegistration> registered_forces;
    void add(GameObject* obj, std::shared_ptr<ForceGenerator> force_generator);
    void remove(GameObject* obj, std::shared_ptr<ForceGenerator> force_generator);
    std::vector<std::shared_ptr<ForceGenerator>> object_forces(GameObject* obj) const;
    void clear();
    void update_forces(linkit::real dt);
};

#endif //VECTRA_FORCE_REGISTRY_H