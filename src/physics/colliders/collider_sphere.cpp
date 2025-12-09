#include "vectra/physics/colliders/collider_sphere.h"
#include "vectra/physics/collision_handler.h"

ColliderSphere::ColliderSphere(const Transform& transform, linkit::real radius) : ColliderPrimitive(transform), radius(radius) {};

std::unique_ptr<ColliderPrimitive> ColliderSphere::clone() const
{
    return std::make_unique<ColliderSphere>(transform, radius);
}

void ColliderSphere::collide_with(ColliderPrimitive& other, CollisionHandler& handler)
{
    other.collide_with_sphere(*this, handler);
}

void ColliderSphere::collide_with_sphere(ColliderSphere& sphere, CollisionHandler& handler)
{
    handler.solve_collision(*this, sphere);
}

void ColliderSphere::collide_with_box(ColliderBox& box, CollisionHandler& handler)
{
    handler.solve_collision(*this, box);
}