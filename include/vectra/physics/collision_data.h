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
        void add_contact(const CollisionContact& contact, linkit::real contact_restitution);
        [[nodiscard]] const std::vector<CollisionContact>& get_contacts() const;
        void set_objects(GameObject* obj1, GameObject* obj2);
        void set_restitution(linkit::real restitution_value);
        [[nodiscard]] bool has_duplicate_contact(const CollisionContact& contact, linkit::real tolerance = 0.01f) const;

        GameObject* objects[2] = {nullptr, nullptr};
        bool valid = false;
        linkit::real restitution = 0.3f;
        linkit::real friction = 0.4f;  // Default friction coefficient
        std::vector<CollisionContact> contacts;
};

#endif //VECTRA_COLLISION_DATA_H