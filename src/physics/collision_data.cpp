#include "vectra/physics/collision_data.h"

CollisionData::CollisionData() = default;

void CollisionData::add_contact(const CollisionContact& contact)
{
    // Avoid duplicate contacts (same position within tolerance)
    if (has_duplicate_contact(contact))
    {
        return;
    }
    contacts.push_back(contact);
    valid = true;
}

void CollisionData::add_contact(const CollisionContact& contact, linkit::real contact_restitution)
{
    if (has_duplicate_contact(contact))
    {
        return;
    }
    contacts.push_back(contact);
    restitution = contact_restitution;
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

void CollisionData::set_restitution(linkit::real restitution_value)
{
    restitution = restitution_value;
}

bool CollisionData::has_duplicate_contact(const CollisionContact& contact, linkit::real tolerance) const
{
    linkit::real tolerance_squared = tolerance * tolerance;
    for (const auto& existing : contacts)
    {
        linkit::Vector3 diff = existing.collision_point - contact.collision_point;
        if (diff.magnitude_squared() < tolerance_squared)
        {
            return true;
        }
    }
    return false;
}

