#import <memory>

#include "vectra/core/gameobject.h"
#include "vectra/physics/colliders/collider_sphere.h"

GameObject::GameObject()
    : model("resources/models/primitives/sphere.obj", false),
      collider(std::make_unique<ColliderSphere>(Transform(), 1.0))
{
    rb = Rigidbody();
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

ColliderPrimitive& GameObject::get_collider()
{
    return *collider;
}

const ColliderPrimitive& GameObject::get_collider() const
{
    return *collider;
}
