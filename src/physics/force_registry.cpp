#include "vectra/physics/force_registry.h"
#include <algorithm>
#include <iostream>

void ForceRegistry::add(GameObject* obj, std::shared_ptr<ForceGenerator> fg)
{
    registered_forces.push_back({obj, fg});
}

void ForceRegistry::remove(GameObject* obj, std::shared_ptr<ForceGenerator> fg)
{
    registered_forces.erase(
        std::remove_if(registered_forces.begin(), registered_forces.end(),
            [&](const ForceRegistration& reg) {
                return reg.obj == obj && reg.force_generator == fg;
            }),
        registered_forces.end());
}

void ForceRegistry::clear()
{
    registered_forces.clear();
}

void ForceRegistry::update_forces(const std::vector<GameObject>& all_objects, linkit::real dt)
{
    for (auto& reg : registered_forces)
    {
        reg.force_generator->update_force(*reg.obj, all_objects, dt);
    }
}