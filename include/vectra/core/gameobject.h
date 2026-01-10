#ifndef VECTRA_GAMEOBJECT_H
#define VECTRA_GAMEOBJECT_H

#include <memory>
#include <string>

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
        GameObject(GameObject&&) noexcept;
        GameObject& operator=(GameObject&&) noexcept;

        void set_collider_type(const std::string& tag);
        void set_shape(const std::string& shape);
        ColliderPrimitive& get_collider();
        [[nodiscard]] const ColliderPrimitive& get_collider() const;

    };
#endif //VECTRA_GAMEOBJECT_H