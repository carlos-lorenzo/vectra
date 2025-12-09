#ifndef VECTRA_COLLISION_CONTACT_H
#define VECTRA_COLLISION_CONTACT_H

#include "linkit/linkit.h"

struct CollisionContact
{
    linkit::Vector3 collision_point;
    linkit::Vector3 collision_normal;
    linkit::real penetration_depth;

    CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal, linkit::real penetration_depth);
};

#endif //VECTRA_COLLISION_CONTACT_H