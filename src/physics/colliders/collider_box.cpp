#include <iostream>
#include "vectra/physics/colliders/collider_box.h"

ColliderBox::ColliderBox(Transform* transform, const linkit::Vector3& half_sizes)
    : ColliderPrimitive(transform), half_sizes(half_sizes)
{
    tag = "ColliderBox";
    // Computing the 8 vertices of the box
    _init_vertices[0] = linkit::Vector3(-half_sizes.x, -half_sizes.y, -half_sizes.z);
    _init_vertices[1] = linkit::Vector3( half_sizes.x, -half_sizes.y, -half_sizes.z);
    _init_vertices[2] = linkit::Vector3( half_sizes.x,  half_sizes.y, -half_sizes.z);
    _init_vertices[3] = linkit::Vector3(-half_sizes.x,  half_sizes.y, -half_sizes.z);
    _init_vertices[4] = linkit::Vector3(-half_sizes.x, -half_sizes.y,  half_sizes.z);
    _init_vertices[5] = linkit::Vector3( half_sizes.x, -half_sizes.y,  half_sizes.z);
    _init_vertices[6] = linkit::Vector3( half_sizes.x,  half_sizes.y,  half_sizes.z);
    _init_vertices[7] = linkit::Vector3(-half_sizes.x,  half_sizes.y,  half_sizes.z);
}

std::unique_ptr<ColliderPrimitive> ColliderBox::clone() const
{
    return std::make_unique<ColliderBox>(transform, half_sizes);
}

CollisionData ColliderBox::collide_with(ColliderPrimitive& other, CollisionHandler& handler)
{
    if (other.tag == "ColliderSphere")
    {
        return collide_with_sphere(dynamic_cast<ColliderSphere&>(other), handler);
    }
    else if (other.tag == "ColliderBox")
    {
        return collide_with_box(dynamic_cast<ColliderBox&>(other), handler);
    } else
    {
        std::cerr << "Unknown collider type: " << other.tag << std::endl;
    }
    CollisionData collision_data;
    return collision_data;

}

CollisionData ColliderBox::collide_with_sphere(ColliderSphere& sphere, CollisionHandler& handler)
{
    CollisionData result = CollisionHandler::solve_sphere_box(sphere, *this);
    // Flip the normal since solve_sphere_box returns normal pointing from sphere to box,
    // but the caller expects normal from box (this) to sphere
    for (auto& contact : result.contacts)
    {
        contact.collision_normal = contact.collision_normal * -1.0f;
    }
    return result;
}

CollisionData ColliderBox::collide_with_box(ColliderBox& box, CollisionHandler& handler)
{
    return CollisionHandler::solve_box_box(*this, box);
}

std::array<linkit::Vector3, 8> ColliderBox::get_vertices() const
{
    std::array<linkit::Vector3, 8> transformed_vertices;
    // Assuming transform->get_model_matrix() returns a 4x4 matrix
    auto model_matrix = transform->get_model_matrix();

    for (size_t i = 0; i < 8; ++i) {
        // Transform the initial local vertex by the model matrix
        transformed_vertices[i] = model_matrix * _init_vertices[i];
    }
    return transformed_vertices;
}


