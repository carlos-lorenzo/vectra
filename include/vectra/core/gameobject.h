#ifndef VECTRA_GAMEOBJECT_H
#define VECTRA_GAMEOBJECT_H

#include <memory>

#include "vectra/rendering/model.h"
#include "vectra/physics/rigidbody.h"
#include "vectra/physics/collider_primitive.h"

class GameObject
{
    public:
        Rigidbody rb;
        Model model;
        Shader shader;
        std::unique_ptr<ColliderPrimitive> collider; // nullable, owns ColliderSphere/ColliderBox

        GameObject();
        GameObject(const GameObject& other);
        GameObject& operator=(const GameObject& other);
        GameObject(GameObject&&) noexcept = default;
        GameObject& operator=(GameObject&&) noexcept = default;
        ColliderPrimitive& get_collider();
        [[nodiscard]] const ColliderPrimitive& get_collider() const;

    };
#endif //VECTRA_GAMEOBJECT_H