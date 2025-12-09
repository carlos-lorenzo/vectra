#include "vectra/physics/collision_handler.h"
#include <cmath>

CollisionHandler::CollisionHandler() = default;

void CollisionHandler::add_collision(const CollisionData& collision)
{
    collisions.push_back(collision);
}

void CollisionHandler::narrow_phase(std::vector<PotentialContact>& potential_contacts)
{
    for (auto& contact : potential_contacts)
    {
        if (!contact.objects[0] || !contact.objects[1])
        {
            continue;
        }

        ColliderPrimitive& collider_one = contact.objects[0]->get_collider();
        ColliderPrimitive& collider_two = contact.objects[1]->get_collider();
        collider_one.collide_with(collider_two, *this);
    }
}

void CollisionHandler::solve_collision(ColliderPrimitive& first, ColliderPrimitive& second)
{
    first.collide_with(second, *this);
}

void CollisionHandler::solve_collision(ColliderSphere& sphere1, ColliderSphere& sphere2)
{
    const Transform& transform_one = sphere1.get_transform();
    const Transform& transform_two = sphere2.get_transform();

    const linkit::Vector3 delta = transform_two.position - transform_one.position;
    const linkit::real distance_squared = delta.magnitude_squared();
    const linkit::real radius_sum = sphere1.radius + sphere2.radius;
    const linkit::real radius_sum_squared = radius_sum * radius_sum;

    if (distance_squared >= radius_sum_squared)
    {
        return; // No penetration
    }

    linkit::real distance = linkit::real_sqrt(distance_squared);
    if (distance < linkit::REAL_EPSILON)
    {
        distance = linkit::REAL_EPSILON;
    }

    const linkit::Vector3 normal = delta / distance;
    const linkit::real penetration = radius_sum - distance;
    const linkit::Vector3 contact_point = transform_one.position + normal * (sphere1.radius - 0.5f * penetration);

    CollisionContact contact(contact_point, normal, penetration);
    CollisionData collision_data;
    collision_data.add_contact(contact);
    add_collision(collision_data);
}

void CollisionHandler::solve_collision(ColliderBox& box1, ColliderBox& box2)
{
    // Not implemented yet
}

void CollisionHandler::solve_contacts()
{
}

void CollisionHandler::clear_contacts()
{
    collisions.clear();
}
