#include "vectra/physics/collision_contact.h"

CollisionContact::CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal, const linkit::real penetration_depth) :
collision_point(collision_point),
collision_normal(collision_normal),
penetration_depth(penetration_depth),
feature(),
contact_velocity(0, 0, 0),
desired_delta_velocity(0)
{
    relative_positions = std::vector<linkit::Vector3>();
}

CollisionContact::CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal,
                                   const linkit::real penetration_depth, const ContactFeature& feature) :
collision_point(collision_point),
collision_normal(collision_normal),
penetration_depth(penetration_depth),
feature(feature),
contact_velocity(0, 0, 0),
desired_delta_velocity(0)
{
    relative_positions = std::vector<linkit::Vector3>();
}

using namespace linkit;
Matrix3 CollisionContact::contact_basis_to_world() const
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

Matrix3 CollisionContact::contact_basis_to_world_inverse() const
{
    Matrix3 contact_to_world = contact_basis_to_world();
    // Rotation matrix
    return contact_to_world.transposed();
}

void CollisionContact::calculate_desired_delta_velocity(linkit::real restitution)
{
    // contact_velocity.x is the separating velocity along the contact normal
    // Positive means separating, negative means closing

    // If objects are already separating, no impulse needed
    if (contact_velocity.x >= 0)
    {
        desired_delta_velocity = 0;
        return;
    }

    // If the velocity is very slow, limit the restitution to avoid jitter
    constexpr linkit::real velocity_limit = 0.25f;
    linkit::real effective_restitution = restitution;
    if (real_abs(contact_velocity.x) < velocity_limit)
    {
        effective_restitution = 0.0f;
    }

    // We want to reverse the closing velocity and add bounce
    // current velocity is negative (closing), we want positive (separating)
    // desired = -current * (1 + restitution)
    desired_delta_velocity = -contact_velocity.x * (1.0f + effective_restitution);
}

