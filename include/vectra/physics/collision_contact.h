// File: `include/vectra/physics/collision_contact.h`
#ifndef VECTRA_COLLISION_CONTACT_H
#define VECTRA_COLLISION_CONTACT_H

#include "linkit/linkit.h"
#include <vector>

struct CollisionContact
{
    linkit::Vector3 collision_point;
    linkit::Vector3 collision_normal;
    linkit::real penetration_depth;
    std::vector<linkit::Vector3> relative_positions;


    CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal, linkit::real penetration_depth);
    [[nodiscard]] linkit::Matrix3 contact_basis_to_world() const;
    [[nodiscard]] linkit::Matrix3 contact_basis_to_world_inverse() const;
};

#endif //VECTRA_COLLISION_CONTACT_H
