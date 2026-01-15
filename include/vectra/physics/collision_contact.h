// File: `include/vectra/physics/collision_contact.h`
#ifndef VECTRA_COLLISION_CONTACT_H
#define VECTRA_COLLISION_CONTACT_H

#include "linkit/linkit.h"
#include <vector>

// Contact feature types for coherence tracking and prioritization
enum class ContactFeatureType
{
    VERTEX_FACE,    // High priority
    EDGE_EDGE,      // High priority
    FACE_FACE,      // Medium priority (generates multiple contacts)
    EDGE_FACE,      // Medium priority
    UNKNOWN         // Default/fallback
};

struct ContactFeature
{
    ContactFeatureType type = ContactFeatureType::UNKNOWN;
    int body_a_feature = -1;  // Index of feature on body A (vertex/edge/face)
    int body_b_feature = -1;  // Index of feature on body B (vertex/edge/face)

    bool operator==(const ContactFeature& other) const
    {
        return type == other.type &&
               body_a_feature == other.body_a_feature &&
               body_b_feature == other.body_b_feature;
    }
};

struct CollisionContact
{
    linkit::Vector3 collision_point;
    linkit::Vector3 collision_normal;
    linkit::real penetration_depth;
    std::vector<linkit::Vector3> relative_positions;
    ContactFeature feature;  // For coherence tracking

    // Velocity resolution data (computed during prepare phase)
    linkit::Vector3 contact_velocity;       // Relative velocity in contact space
    linkit::real desired_delta_velocity;    // Target velocity change for resolution

    CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal, linkit::real penetration_depth);
    CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal,
                     linkit::real penetration_depth, const ContactFeature& feature);
    [[nodiscard]] linkit::Matrix3 contact_basis_to_world() const;
    [[nodiscard]] linkit::Matrix3 contact_basis_to_world_inverse() const;
    void calculate_desired_delta_velocity(linkit::real restitution);
};

#endif //VECTRA_COLLISION_CONTACT_H
