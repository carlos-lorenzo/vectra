#include "vectra/physics/collision_handler.h"
#include <cmath>
#include <iostream>
#include <ostream>

CollisionHandler::CollisionHandler() = default;

void CollisionHandler::add_collision(const CollisionData& collision)
{
    collisions.push_back(collision);
}

void CollisionHandler::narrow_phase(const std::vector<PotentialContact>& potential_contacts)
{
    for (const auto& [objects] : potential_contacts)
    {
        if (!objects[0] || !objects[1])
        {
            continue;
        }

        ColliderPrimitive& collider_one = objects[0]->get_collider();
        ColliderPrimitive& collider_two = objects[1]->get_collider();

        solve_collision(collider_one, collider_two);
    }
}

void CollisionHandler::solve_collision(ColliderPrimitive& first, ColliderPrimitive& second)
{
    first.collide_with(second, *this);
}


void CollisionHandler::solve_sphere_sphere(const ColliderSphere& first, const ColliderSphere& second)
{
    const Transform& transform_one = first.get_transform();
    const Transform& transform_two = second.get_transform();

    const linkit::Vector3 delta = transform_two.position - transform_one.position;
    const linkit::real distance_squared = delta.magnitude_squared();
    const linkit::real radius_sum = first.radius + second.radius;
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
    const linkit::Vector3 contact_point = transform_one.position + normal * (first.radius - 0.5f * penetration);

    CollisionContact contact(contact_point, normal, penetration);
    CollisionData collision_data;
    collision_data.add_contact(contact);
    add_collision(collision_data);
}

void CollisionHandler::solve_box_box(ColliderBox& first, ColliderBox& second)
{
}

void CollisionHandler::solve_sphere_box(ColliderSphere& sphere, ColliderBox& box)
{

}

void CollisionHandler::solve_contacts()
{
}

void CollisionHandler::clear_contacts()
{
    collisions.clear();
}
