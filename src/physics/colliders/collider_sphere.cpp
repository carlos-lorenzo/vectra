#include <iostream>
#include "vectra/physics/colliders/collider_sphere.h"
#include "vectra/physics/collision_handler.h"

ColliderSphere::ColliderSphere(Transform* transform, const linkit::real radius)
    : ColliderPrimitive(transform), radius(radius)
{
    tag = "ColliderSphere";
}

std::unique_ptr<ColliderPrimitive> ColliderSphere::clone() const
{
    return std::make_unique<ColliderSphere>(transform, radius);
}

void ColliderSphere::collide_with(ColliderPrimitive& other, CollisionHandler& handler)
{

    if (other.tag == "ColliderSphere")
    {
        collide_with_sphere(dynamic_cast<ColliderSphere&>(other), handler);
    }
    else if (other.tag == "ColliderBox")
    {
        collide_with_box(dynamic_cast<ColliderBox&>(other), handler);
    }
    else
    {
        std::cerr << "Unknown collider type: " << other.tag << std::endl;
    }
}

void ColliderSphere::collide_with_sphere(ColliderSphere& sphere, CollisionHandler& handler)
{
    handler.solve_sphere_sphere(*this, sphere);
}

void ColliderSphere::collide_with_box(ColliderBox& box, CollisionHandler& handler)
{
    handler.solve_sphere_box(*this, box);
}
