#ifndef VECTRA_COLLIDER_PRIMITIVE_H
#define VECTRA_COLLIDER_PRIMITIVE_H

#include <memory>
#include <string>
#include "vectra/physics/transform.h"

class ColliderSphere;
class ColliderBox;
class CollisionHandler;

class ColliderPrimitive
{
protected:
    Transform* transform;
    explicit ColliderPrimitive(Transform* transform);
    ColliderPrimitive(const ColliderPrimitive&) = default;
    ColliderPrimitive& operator=(const ColliderPrimitive&) = default;
    ColliderPrimitive(ColliderPrimitive&&) noexcept = default;
    ColliderPrimitive& operator=(ColliderPrimitive&&) noexcept = default;

public:
    std::string tag;
    virtual ~ColliderPrimitive() = default;
    [[nodiscard]] const Transform& get_transform() const;

    // Add this setter
    void set_transform(Transform* new_transform);

    [[nodiscard]] virtual std::unique_ptr<ColliderPrimitive> clone() const = 0;
    virtual void collide_with(ColliderPrimitive& other, CollisionHandler& handler) = 0;
    virtual void collide_with_sphere(ColliderSphere& sphere, CollisionHandler& handler) = 0;
    virtual void collide_with_box(ColliderBox& box, CollisionHandler& handler) = 0;
};

#endif //VECTRA_COLLIDER_PRIMITIVE_H