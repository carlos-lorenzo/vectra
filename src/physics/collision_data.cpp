#include "vectra/physics/collision_data.h"

CollisionData::CollisionData() = default;

void CollisionData::add_contact(const CollisionContact& contact)
{
    contacts.push_back(contact);
    restitution = 0;  // Reasonable default restitution for bouncy collisions
    valid = true;
}

const std::vector<CollisionContact>& CollisionData::get_contacts() const
{
    return contacts;
}

void CollisionData::set_objects(GameObject* obj1, GameObject* obj2)
{
    objects[0] = obj1;
    objects[1] = obj2;
}
