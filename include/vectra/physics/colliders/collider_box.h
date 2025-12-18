#ifndef VECTRA_COLLIDER_BOX_H
#define VECTRA_COLLIDER_BOX_H

#include "linkit/linkit.h"
#include "vectra/physics/collider_primitive.h"
#include "vectra/physics/collision_handler.h"
class ColliderBox : public ColliderPrimitive
{
public:
    linkit::Vector3 half_sizes;
    linkit::Vector3 _init_vertices[8];
    ColliderBox(Transform* transform, const linkit::Vector3& half_sizes);
    [[nodiscard]] std::unique_ptr<ColliderPrimitive> clone() const override;
    CollisionData collide_with(ColliderPrimitive& other, CollisionHandler& handler) override;
    CollisionData collide_with_sphere(ColliderSphere& sphere, CollisionHandler& handler) override;
    CollisionData collide_with_box(ColliderBox& box, CollisionHandler& handler) override;
    [[nodiscard]] std::array<linkit::Vector3, 8> get_vertices() const;

};

#endif //VECTRA_COLLIDER_BOX_H
