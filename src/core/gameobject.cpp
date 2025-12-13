#include <memory>
#include <iostream>
#include "vectra/core/gameobject.h"
#include "vectra/physics/colliders/collider_sphere.h"

GameObject::GameObject()
    :
    rb(Rigidbody()),
    model("resources/models/primitives/sphere.obj", false),
    collider(std::make_unique<ColliderSphere>(&rb.transform, 1.0))
{
    shader = Shader("resources/shaders/model.vert", "resources/shaders/phong.frag");
}

GameObject::GameObject(const GameObject& other)
    : rb(other.rb),
      model(other.model),
      shader(other.shader)
{
    if (other.collider)
    {
        collider = other.collider->clone();
    }
}

GameObject& GameObject::operator=(const GameObject& other)
{
    if (this == &other) return *this;
    rb = other.rb;
    model = other.model;
    shader = other.shader;
    collider = other.collider ? other.collider->clone() : nullptr;
    return *this;
}

void GameObject::set_collider_type(const std::string& tag)
{
    if (tag == "ColliderSphere")
    {
        collider = std::make_unique<ColliderSphere>(&rb.transform, rb.transform.size());
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
    model = Model("resources/models/primitives/" + shape + ".obj", false);
}

ColliderPrimitive& GameObject::get_collider()
{
    return *collider;
}

const ColliderPrimitive& GameObject::get_collider() const
{
    return *collider;
}
