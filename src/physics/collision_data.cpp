#include "vectra/physics/collision_data.h"

CollisionData::CollisionData() = default;

void CollisionData::add_contact(const CollisionContact& contact)
{
    contacts.push_back(contact);
}

const std::vector<CollisionContact>& CollisionData::get_contacts() const
{
    return contacts;
}
