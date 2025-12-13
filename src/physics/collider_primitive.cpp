#include "vectra/physics/collider_primitive.h"
#include "vectra/core/gameobject.h"

ColliderPrimitive::ColliderPrimitive(Transform* transform)
    : transform(transform)
{
}

const Transform& ColliderPrimitive::get_transform() const
{
    return *transform;
}

void ColliderPrimitive::set_transform(Transform* new_transform)
{
    transform = new_transform;
}
