#ifndef VECTRA_COLLISION_DATA_H
#define VECTRA_COLLISION_DATA_H

#include <vector>
#include "linkit/linkit.h"

#include "vectra/physics/collision_contact.h"


class GameObject;
class CollisionData
{
    public:
        CollisionData();
        void add_contact(const CollisionContact& contact);
        [[nodiscard]] const std::vector<CollisionContact>& get_contacts() const;
        void set_objects(GameObject* obj1, GameObject* obj2);
        GameObject* objects[2];
        bool valid; // Required to allow for collision checks to return no collisions
        linkit::real restitution;
        std::vector<CollisionContact> contacts;

    // Will include other parameters such as restitution once I get narrow phase detection working
};

#endif //VECTRA_COLLISION_DATA_H