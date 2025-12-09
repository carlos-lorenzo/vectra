

#ifndef VECTRA_COLLIDER_BOX_H
#define VECTRA_COLLIDER_BOX_H

#include "linkit/linkit.h"
#include "vectra/physics/collider_primitive.h"

class ColliderBox : public ColliderPrimitive
{
public:
    linkit::Vector3 half_sizes;
    ColliderBox(Transform transform, linkit::Vector3 half_sizes);
};


#endif //VECTRA_COLLIDER_BOX_H
