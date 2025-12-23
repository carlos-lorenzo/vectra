#include "vectra/physics/collision_contact.h"

CollisionContact::CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal, const linkit::real penetration_depth) :
collision_point(collision_point),
collision_normal(collision_normal),
penetration_depth(penetration_depth) {}

using namespace linkit;
Matrix3 CollisionContact::contact_basis_to_world()
{

    Vector3 contact_tangents[2];

    if (real_abs(collision_normal.x) > real_abs(collision_normal.y))
    {
        const real scaling_factor = real_inverse(real_sqrt(collision_normal.z*collision_normal.z + collision_normal.x*collision_normal.x));
        contact_tangents[0] = Vector3(collision_normal.z * scaling_factor, 0,
                                      -collision_normal.x * scaling_factor);
        contact_tangents[1] = Vector3(collision_normal.y*contact_tangents[0].x,
                                      collision_normal.z*contact_tangents[0].x - collision_normal.x*contact_tangents[0].z,
                                      -collision_normal.y*contact_tangents[0].x);


    } else
    {
        const real scaling_factor = real_inverse(real_sqrt(collision_normal.z*collision_normal.z + collision_normal.y*collision_normal.y));
        contact_tangents[0] = Vector3(0,
                                      -collision_normal.z * scaling_factor,
                                      collision_normal.y * scaling_factor);
        contact_tangents[1] = Vector3(collision_normal.y*contact_tangents[0].z - collision_normal.z*contact_tangents[0].y,
                                      -collision_normal.x*contact_tangents[0].z,
                                      collision_normal.x*contact_tangents[0].y);
    }

    return Matrix3::matrix_from_columns(collision_normal, contact_tangents[0], contact_tangents[1]);

}

Matrix3 CollisionContact::contact_basis_to_world_inverse()
{
    Matrix3 contact_to_world = contact_basis_to_world();
    // Rotation matrix
    return contact_to_world.transposed();
}

