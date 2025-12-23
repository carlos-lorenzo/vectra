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
        void narrow_phase(const std::vector<PotentialContact>& potential_contacts);

        CollisionData solve_collision(ColliderPrimitive& first, ColliderPrimitive& second);
        static CollisionData solve_sphere_sphere(const ColliderSphere& first, const ColliderSphere& second);
        static CollisionData solve_box_box(ColliderBox& first, ColliderBox& second);
        static CollisionData solve_sphere_box(ColliderSphere& sphere, ColliderBox& box);
        void solve_contacts();
        void resolve_interpretations();
        void clear_contacts();

    std::vector<CollisionData> collisions;
};

#endif //VECTRA_COLLISION_HANDLER_H
