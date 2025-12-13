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

void ColliderBox::collide_with(ColliderPrimitive& other, CollisionHandler& handler)
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
        // Handle unknown collider type
        std::cerr << "Unknown collider type: " << other.tag << std::endl;
    }
}

void ColliderBox::collide_with_sphere(ColliderSphere& sphere, CollisionHandler& handler)
{
    handler.solve_sphere_box(sphere, *this);
}

void ColliderBox::collide_with_box(ColliderBox& box, CollisionHandler& handler)
{
    handler.solve_box_box(*this, box);
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


