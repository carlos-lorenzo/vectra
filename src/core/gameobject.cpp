#include <memory>
#include <iostream>
#include "vectra/core/gameobject.h"
#include "vectra/physics/colliders/collider_sphere.h"
#include "vectra/physics/colliders/collider_box.h"

GameObject::GameObject()
    :
    rb(Rigidbody()),
    collider(std::make_unique<ColliderSphere>(&rb.transform, 1.0))
{
    model_name = "sphere";
}

GameObject::GameObject(const GameObject& other)
    : rb(other.rb),
      model_name(other.model_name)
{
    if (other.collider)
    {
        collider = other.collider->clone();
        collider->set_transform(&rb.transform);
    }
}

GameObject& GameObject::operator=(const GameObject& other)
{
    if (this == &other) return *this;
    rb = other.rb;
    model_name = other.model_name;
    collider = other.collider ? other.collider->clone() : nullptr;
    if (collider)
    {
        collider->set_transform(&rb.transform);
    }
    return *this;
}

GameObject::GameObject(GameObject&& other) noexcept
    : rb(std::move(other.rb)),
      model_name(std::move(other.model_name)),
      collider(std::move(other.collider))
{
    if (collider)
    {
        collider->set_transform(&rb.transform);
    }
}

GameObject& GameObject::operator=(GameObject&& other) noexcept
{
    if (this == &other) return *this;
    rb = std::move(other.rb);
    model_name = std::move(other.model_name);
    collider = std::move(other.collider);

    if (collider)
    {
        collider->set_transform(&rb.transform);
    }
    return *this;
}

void GameObject::set_collider_type(const std::string& tag)
{
    if (tag == "ColliderSphere")
    {
        linkit::real radius = (rb.transform.scale.x + rb.transform.scale.y + rb.transform.scale.z) / 3.0f;
        collider = std::make_unique<ColliderSphere>(&rb.transform, radius);

    }
    else if (tag == "ColliderBox")
    {
        // Example: create a box collider with half-sizes of 1.0
        collider = std::make_unique<ColliderBox>(&rb.transform, rb.transform.scale);
    }
    else
    {
        std::cerr << "Unknown collider type: " << tag << std::endl;
    }
    // Additional collider types can be added here
}

void GameObject::set_shape(const std::string& shape)
{
    model_name = shape;
}

ColliderPrimitive& GameObject::get_collider()
{
    return *collider;
}

const ColliderPrimitive& GameObject::get_collider() const
{
    return *collider;
}
