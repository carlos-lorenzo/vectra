#ifndef VECTRA_COLLISION_HANDLER_H
#define VECTRA_COLLISION_HANDLER_H

#include <vector>

#include "vectra/physics/BVHNode.h"
#include "vectra/physics/collision_contact.h"
#include "vectra/physics/collision_data.h"

// Collider imports - handler will implement collision checking between every possible pair
#include "vectra/physics/colliders/collider_sphere.h"
#include "vectra/physics/colliders/collider_box.h"

class CollisionHandler
{
public:
    CollisionHandler();

    void add_collision(const CollisionData& collision);
    void narrow_phase(std::vector<PotentialContact>& potential_contacts);

    void solve_collision(ColliderPrimitive& first, ColliderPrimitive& second);
    void solve_collision(ColliderSphere& first, ColliderSphere& second);
    void solve_collision(ColliderBox& first, ColliderBox& second);

    void solve_contacts();
    void clear_contacts();

    std::vector<CollisionData> collisions;
};

#endif //VECTRA_COLLISION_HANDLER_H
