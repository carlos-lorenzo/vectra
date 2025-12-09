#ifndef VECTRA_COLLIDER_SPHERE_H
#define VECTRA_COLLIDER_SPHERE_H

#include <memory>
#include "linkit/linkit.h"
#include "vectra/physics/collision_handler.h"
#include "vectra/physics/collider_primitive.h"

class ColliderSphere : public ColliderPrimitive
{
public:
    linkit::real radius;
    ColliderSphere(const Transform& transform, linkit::real radius);

    [[nodiscard]] std::unique_ptr<ColliderPrimitive> clone() const override;
    void collide_with(ColliderPrimitive& other, CollisionHandler& handler) override;
    void collide_with_sphere(ColliderSphere& sphere, CollisionHandler& handler) override;
    void collide_with_box(ColliderBox& box, CollisionHandler& handler) override;
};


#endif //VECTRA_COLLIDER_SPHERE_H
