#include "vectra/physics/collision_handler.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <unistd.h>
#include <algorithm>
#include <array>

CollisionHandler::CollisionHandler() = default;

void CollisionHandler::add_collision(const CollisionData& collision)
{
    collisions.push_back(collision);
}

void CollisionHandler::narrow_phase(const std::vector<PotentialContact>& potential_contacts)
{
    for (const auto& [objects] : potential_contacts)
    {
        if (!objects[0] || !objects[1])
        {
            continue;
        }

        ColliderPrimitive& collider_one = objects[0]->get_collider();
        ColliderPrimitive& collider_two = objects[1]->get_collider();

        CollisionData collision_data = solve_collision(collider_one, collider_two);
        if (collision_data.valid)
        {
            for (auto &contact : collision_data.contacts)
            {
                // Relative position is FROM body center TO contact point
                contact.relative_positions = {
                    (contact.collision_point - objects[0]->rb.transform.position),
                    (contact.collision_point - objects[1]->rb.transform.position)
                };

            }
            collision_data.set_objects(objects[0], objects[1]);
            add_collision(collision_data);
        }
    }

}

CollisionData CollisionHandler::solve_collision(ColliderPrimitive& first, ColliderPrimitive& second)
{
    return first.collide_with(second, *this);
}


CollisionData CollisionHandler::solve_sphere_sphere(const ColliderSphere& first, const ColliderSphere& second)
{
    CollisionData collision_data;
    const Transform& transform_one = first.get_transform();
    const Transform& transform_two = second.get_transform();

    const linkit::Vector3 delta = transform_two.position - transform_one.position;
    const linkit::real distance_squared = delta.magnitude_squared();
    const linkit::real radius_sum = first.radius + second.radius;
    const linkit::real radius_sum_squared = radius_sum * radius_sum;



    if (distance_squared >= radius_sum_squared)
    {
        return collision_data; // No penetration
    }
    linkit::real distance = linkit::real_sqrt(distance_squared);

    if (distance < linkit::REAL_EPSILON)
    {
        distance = linkit::REAL_EPSILON;
    }

    const linkit::Vector3 normal = delta / distance;
    const linkit::real penetration = radius_sum - distance;
    const linkit::Vector3 contact_point = transform_one.position + normal * (first.radius - 0.5f * penetration);

    CollisionContact contact(contact_point, normal, penetration);
    collision_data.add_contact(contact);
    return collision_data;

}

CollisionData CollisionHandler::solve_box_box(ColliderBox& first, ColliderBox& second)
{
    CollisionData collision_data;

    BoxAxes axes1 = get_box_axes(first.get_transform());
    BoxAxes axes2 = get_box_axes(second.get_transform());

    linkit::Vector3 to_center = axes2.center - axes1.center;

    linkit::real min_overlap = 1e20f;
    int best_case = -1;
    linkit::Vector3 best_axis;

    // Track overlaps for each axis type to determine contact type
    linkit::real face_overlap_1 = 1e20f;
    linkit::real face_overlap_2 = 1e20f;
    linkit::real edge_overlap = 1e20f;
    int best_face_1 = -1;
    int best_face_2 = -1;
    int best_edge_1 = -1;
    int best_edge_2 = -1;

    // Helper to test a separating axis
    auto test_axis = [&](const linkit::Vector3& axis, int case_index) -> bool {
        linkit::real axis_mag_sq = axis.magnitude_squared();
        if (axis_mag_sq < 1e-6f) return true; // Skip degenerate axes

        linkit::Vector3 n = axis;
        n.normalize();

        // Project half-sizes of box 1 onto normal
        linkit::real r1 = first.half_sizes.x * std::abs(axes1.axes[0] * n) +
                          first.half_sizes.y * std::abs(axes1.axes[1] * n) +
                          first.half_sizes.z * std::abs(axes1.axes[2] * n);

        // Project half-sizes of box 2 onto normal
        linkit::real r2 = second.half_sizes.x * std::abs(axes2.axes[0] * n) +
                          second.half_sizes.y * std::abs(axes2.axes[1] * n) +
                          second.half_sizes.z * std::abs(axes2.axes[2] * n);

        linkit::real dist = std::abs(to_center * n);
        linkit::real overlap = (r1 + r2) - dist;

        if (overlap < 0) return false; // Separating axis found

        // Track best axis per category
        if (case_index < 3) {
            // Face normals of box 1
            if (overlap < face_overlap_1) {
                face_overlap_1 = overlap;
                best_face_1 = case_index;
            }
        } else if (case_index < 6) {
            // Face normals of box 2
            if (overlap < face_overlap_2) {
                face_overlap_2 = overlap;
                best_face_2 = case_index - 3;
            }
        } else {
            // Edge-edge axes
            if (overlap < edge_overlap) {
                edge_overlap = overlap;
                best_edge_1 = (case_index - 6) / 3;
                best_edge_2 = (case_index - 6) % 3;
            }
        }

        if (overlap < min_overlap) {
            min_overlap = overlap;
            best_axis = n;
            best_case = case_index;
        }
        return true;
    };

    // 1. Test face normals of box 1 (cases 0-2)
    for (int i = 0; i < 3; ++i) {
        if (!test_axis(axes1.axes[i], i)) return collision_data;
    }

    // 2. Test face normals of box 2 (cases 3-5)
    for (int i = 0; i < 3; ++i) {
        if (!test_axis(axes2.axes[i], i + 3)) return collision_data;
    }

    // 3. Test cross-products of edges (cases 6-14)
    // Add bias to prefer face contacts over edge contacts for stability
    constexpr linkit::real edge_bias = 0.95f;
    linkit::real face_min = std::min(face_overlap_1, face_overlap_2);

    int case_idx = 6;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            linkit::Vector3 cross_axis(
                axes1.axes[i].y * axes2.axes[j].z - axes1.axes[i].z * axes2.axes[j].y,
                axes1.axes[i].z * axes2.axes[j].x - axes1.axes[i].x * axes2.axes[j].z,
                axes1.axes[i].x * axes2.axes[j].y - axes1.axes[i].y * axes2.axes[j].x
            );
            if (!test_axis(cross_axis, case_idx)) return collision_data;
            case_idx++;
        }
    }

    // Ensure normal points from box 1 to box 2
    if (best_axis * to_center < 0) {
        best_axis = best_axis * -1.0f;
    }

    // Determine contact type and generate appropriate contacts
    if (best_case < 3) {
        // Face of box 1 is the reference face
        generate_face_contacts(first, second, axes1, axes2, best_case, false, min_overlap, collision_data);
    }
    else if (best_case < 6) {
        // Face of box 2 is the reference face
        generate_face_contacts(second, first, axes2, axes1, best_case - 3, true, min_overlap, collision_data);
    }
    else {
        // Edge-edge contact - prefer face contact if penetrations are close (stability bias)
        if (edge_overlap > face_min * edge_bias && face_min < 1e19f) {
            // Use face contact instead for better stability
            if (face_overlap_1 <= face_overlap_2) {
                generate_face_contacts(first, second, axes1, axes2, best_face_1, false, face_overlap_1, collision_data);
            } else {
                generate_face_contacts(second, first, axes2, axes1, best_face_2, true, face_overlap_2, collision_data);
            }
        } else {
            // True edge-edge contact
            generate_edge_edge_contact(first, second, axes1, axes2, best_edge_1, best_edge_2, best_axis, min_overlap, collision_data);
        }
    }

    return collision_data;
}


CollisionData CollisionHandler::solve_sphere_box(ColliderSphere& sphere, ColliderBox& box)
{
    CollisionData collision_data;
    const Transform& sphere_transform = sphere.get_transform();
    const Transform& box_transform = box.get_transform();

    // Get the box's model matrix to handle rotation (OBB)
    auto box_model = box_transform.get_model_matrix();
    linkit::Vector3 box_center = box_transform.position;

    // Extract the box's local axes from the model matrix
    // Assuming Vector3 is treated as a point (w=1), M * (1,0,0) = center + axis_x
    linkit::Vector3 axis_x = box_model * linkit::Vector3(1, 0, 0) - box_center;
    linkit::Vector3 axis_y = box_model * linkit::Vector3(0, 1, 0) - box_center;
    linkit::Vector3 axis_z = box_model * linkit::Vector3(0, 0, 1) - box_center;

    axis_x.normalize();
    axis_y.normalize();
    axis_z.normalize();

    // Transform the sphere center into the box's local coordinate space
    linkit::Vector3 rel_center = sphere_transform.position - box_center;
    linkit::Vector3 local_center(
        rel_center * axis_x,
        rel_center * axis_y,
        rel_center * axis_z
    );

    // Find the closest point on the box to the sphere center in local space
    // Since we are in local space, the box is axis-aligned
    linkit::Vector3 closest_local;
    closest_local.x = std::clamp(local_center.x, -box.half_sizes.x, box.half_sizes.x);
    closest_local.y = std::clamp(local_center.y, -box.half_sizes.y, box.half_sizes.y);
    closest_local.z = std::clamp(local_center.z, -box.half_sizes.z, box.half_sizes.z);

    // Transform the closest point back to world space
    linkit::Vector3 closest_world = box_center +
                                    axis_x * closest_local.x +
                                    axis_y * closest_local.y +
                                    axis_z * closest_local.z;

    // Calculate the vector from the sphere center to the closest point on the box
    const linkit::Vector3 delta = closest_world - sphere_transform.position;
    const linkit::real distance_squared = delta.magnitude_squared();
    const linkit::real radius_squared = sphere.radius * sphere.radius;
    if (distance_squared >= radius_squared)
    {
        return collision_data; // No penetration
    }

    linkit::real distance = linkit::real_sqrt(distance_squared);
    if (distance < linkit::REAL_EPSILON)
    {
        distance = linkit::REAL_EPSILON;
    }

    const linkit::Vector3 normal = delta / distance;
    const linkit::real penetration = sphere.radius - distance;
    const linkit::Vector3 contact_point = sphere_transform.position + normal * (sphere.radius - 0.5f * penetration);

    const CollisionContact contact(contact_point, normal, penetration);
    collision_data.add_contact(contact);
    return collision_data;
}


void CollisionHandler::solve_contacts()
{
    if (collisions.empty()) return;
    for (auto &collision : collisions)
    {

        for (auto& contact : collision.get_contacts())
        {
            linkit::real delta_velocity = 0;
            linkit::Vector3 closing_velocity = linkit::Vector3(0, 0, 0);

            for (int i=0; i<2; i++)
            {
                linkit::Matrix3 inverse_inertia_tensor = collision.objects[i]->rb._local_inverse_inertia_tensor;

                linkit::Vector3 torque = contact.relative_positions[i] % contact.collision_normal;
                linkit::Vector3 delta_angular_velocity = inverse_inertia_tensor * torque;
                delta_velocity += (delta_angular_velocity % contact.relative_positions[i]) * contact.collision_normal;
                delta_velocity += collision.objects[i]->rb.inverse_mass;

                // Calculate velocity at contact point for this body
                linkit::Vector3 velocity_at_contact = collision.objects[i]->rb.velocity +
                    collision.objects[i]->rb.angular_velocity % contact.relative_positions[i];

                // First object adds, second object subtracts (closing velocity)
                if (i == 0) {
                    closing_velocity += velocity_at_contact;
                } else {
                    closing_velocity -= velocity_at_contact;
                }
            }

            // Separating velocity along the normal (rate at which objects separate)
            // closing_velocity = v0 - v1, so separating = (v1 - v0) · n = -closing · n
            linkit::real separating_velocity = -(closing_velocity * contact.collision_normal);

            // If objects are already separating, skip
            if (separating_velocity > 0) continue;

            linkit::real desired_delta_velocity = -separating_velocity * (1 + collision.restitution);

            // Avoid division by zero
            if (delta_velocity < linkit::REAL_EPSILON) continue;

            linkit::real impulse_magnitude = desired_delta_velocity / delta_velocity;

            // Safety check for NaN/Inf
            if (!std::isfinite(impulse_magnitude)) continue;

            linkit::Vector3 impulse = contact.collision_normal * impulse_magnitude;

            // Apply impulse to first object (pushed away from object 1, opposite to normal)
            {
                linkit::Vector3 velocity_change = impulse * collision.objects[0]->rb.inverse_mass;
                linkit::Vector3 impulse_torque = contact.relative_positions[0] % impulse;
                linkit::Vector3 rotation_change = collision.objects[0]->rb._local_inverse_inertia_tensor * impulse_torque;

                if (std::isfinite(velocity_change.x) && std::isfinite(velocity_change.y) && std::isfinite(velocity_change.z))
                {
                    collision.objects[0]->rb.velocity -= velocity_change;
                }
                if (std::isfinite(rotation_change.x) && std::isfinite(rotation_change.y) && std::isfinite(rotation_change.z))
                {
                    collision.objects[0]->rb.angular_velocity -= rotation_change;
                }
            }

            // Apply impulse to the second object (pushed away from object 0, along normal)
            {
                linkit::Vector3 velocity_change = impulse * collision.objects[1]->rb.inverse_mass;
                linkit::Vector3 impulse_torque = contact.relative_positions[1] % impulse;
                linkit::Vector3 rotation_change = collision.objects[1]->rb._local_inverse_inertia_tensor * impulse_torque;

                if (std::isfinite(velocity_change.x) && std::isfinite(velocity_change.y) && std::isfinite(velocity_change.z))
                {
                    collision.objects[1]->rb.velocity += velocity_change;
                }
                if (std::isfinite(rotation_change.x) && std::isfinite(rotation_change.y) && std::isfinite(rotation_change.z))
                {
                    collision.objects[1]->rb.angular_velocity += rotation_change;
                }
            }
        }
    }

}

void CollisionHandler::resolve_interpretations()
{
    // Non-linear interpretation resolution -> might implement relaxation in the future
    if (collisions.empty()) return;
    for (auto &collision : collisions)
    {
        for (auto& contact : collision.get_contacts())
        {
            linkit::real angular_inertia_contact[2];
            linkit::real angular_move[2];

            linkit::real linear_inertia_contact[2];
            linkit::real linear_move[2];

            linkit::real total_inertia = 0;
            for (int i=0; i<2;i++)
            {
                GameObject* body = collision.objects[i];
                linkit::Matrix3 inverse_inertia_tensor = body->rb._local_inverse_inertia_tensor;

                linkit::Vector3 torque = contact.relative_positions[i] % contact.collision_normal;
                linkit::Vector3 delta_angular_velocity = inverse_inertia_tensor * torque;
                linkit::Vector3 angular_inertia_world = (delta_angular_velocity % contact.relative_positions[i]);
                angular_inertia_contact[i] = angular_inertia_world * contact.collision_normal;
                linear_inertia_contact[i] = body->rb.inverse_mass;
                total_inertia += angular_inertia_contact[i] + linear_inertia_contact[i];
            }

            // Avoid division by zero
            if (total_inertia < linkit::REAL_EPSILON) continue;

            const linkit::real inverse_total_inertia = 1.0f / total_inertia;


            angular_move[0] = -contact.penetration_depth * angular_inertia_contact[0] * inverse_total_inertia;
            angular_move[1] = contact.penetration_depth * angular_inertia_contact[1] * inverse_total_inertia;

            linear_move[0] = -contact.penetration_depth * linear_inertia_contact[0] * inverse_total_inertia;
            linear_move[1] = contact.penetration_depth * linear_inertia_contact[1] * inverse_total_inertia;

            for (int i=0; i<2;i++)
            {
                GameObject* body = collision.objects[i];

                // Angular resolution
                linkit::real limit = 0.2 * contact.relative_positions[i].magnitude();
                if (linkit::real_abs(angular_move[i]) > limit)
                {
                    linkit::real total_move = linear_move[i] + angular_move[i];
                    if (angular_move[i] >= 0)
                    {
                        angular_move[i] = limit;
                    } else
                    {
                        angular_move[i] = -limit;
                    }
                    linear_move[i] = total_move - angular_move[i];
                }

                // Only apply angular resolution if there's angular inertia
                if (linkit::real_abs(angular_inertia_contact[i]) > linkit::REAL_EPSILON)
                {
                    linkit::Matrix3 inverse_inertia_tensor = body->rb._local_inverse_inertia_tensor;
                    linkit::Vector3 impulsive_torque = contact.relative_positions[i] % contact.collision_normal;
                    linkit::Vector3 impulse_per_move = inverse_inertia_tensor * impulsive_torque;
                    linkit::Vector3 rotation_per_move = impulse_per_move * (1.0/angular_inertia_contact[i]);
                    linkit::Vector3 rotation = rotation_per_move * angular_move[i];

                    // Safety check for NaN/Inf
                    if (std::isfinite(rotation.x) && std::isfinite(rotation.y) && std::isfinite(rotation.z))
                    {
                        body->rb.transform.rotation.add_scaled_vector(rotation, 1);
                        body->rb.transform.rotation.normalize();
                    }
                }

                // Linear resolution
                linkit::Vector3 linear_displacement = contact.collision_normal * linear_move[i];
                if (std::isfinite(linear_displacement.x) && std::isfinite(linear_displacement.y) && std::isfinite(linear_displacement.z))
                {
                    body->rb.transform.translate(linear_displacement);
                }


            }
        }
    }

}

void CollisionHandler::clear_contacts()
{
    collisions.clear();
}

// ============================================================================
// Helper functions for multi-point box-box contact generation
// ============================================================================

CollisionHandler::BoxAxes CollisionHandler::get_box_axes(const Transform& tf)
{
    BoxAxes result;
    result.center = tf.position;

    auto model = tf.get_model_matrix();
    result.axes[0] = model * linkit::Vector3(1, 0, 0) - result.center;
    result.axes[1] = model * linkit::Vector3(0, 1, 0) - result.center;
    result.axes[2] = model * linkit::Vector3(0, 0, 1) - result.center;

    result.axes[0].normalize();
    result.axes[1].normalize();
    result.axes[2].normalize();

    return result;
}

std::vector<linkit::Vector3> CollisionHandler::get_face_vertices(
    const ColliderBox& box,
    const BoxAxes& axes,
    int face_index)
{
    // Face indices: 0=+X, 1=+Y, 2=+Z (and implicitly their negatives based on contact direction)
    // Returns the 4 vertices of the face in CCW order when viewed from outside

    std::vector<linkit::Vector3> vertices(4);
    const linkit::Vector3& half = box.half_sizes;

    // Get the two axes that form the face plane
    int axis1 = (face_index + 1) % 3;
    int axis2 = (face_index + 2) % 3;

    linkit::real half_sizes[3] = {half.x, half.y, half.z};

    // The face is at +half_size along the face_index axis
    linkit::Vector3 face_center = axes.center + axes.axes[face_index] * half_sizes[face_index];

    // Generate 4 corners
    vertices[0] = face_center + axes.axes[axis1] * half_sizes[axis1] + axes.axes[axis2] * half_sizes[axis2];
    vertices[1] = face_center - axes.axes[axis1] * half_sizes[axis1] + axes.axes[axis2] * half_sizes[axis2];
    vertices[2] = face_center - axes.axes[axis1] * half_sizes[axis1] - axes.axes[axis2] * half_sizes[axis2];
    vertices[3] = face_center + axes.axes[axis1] * half_sizes[axis1] - axes.axes[axis2] * half_sizes[axis2];

    return vertices;
}

std::vector<linkit::Vector3> CollisionHandler::clip_polygon_against_plane(
    const std::vector<linkit::Vector3>& polygon,
    const linkit::Vector3& plane_normal,
    linkit::real plane_offset)
{
    // Sutherland-Hodgman clipping algorithm
    if (polygon.empty()) return {};

    std::vector<linkit::Vector3> output;
    output.reserve(polygon.size() + 1);

    for (size_t i = 0; i < polygon.size(); ++i)
    {
        const linkit::Vector3& current = polygon[i];
        const linkit::Vector3& next = polygon[(i + 1) % polygon.size()];

        linkit::real current_dist = (current * plane_normal) - plane_offset;
        linkit::real next_dist = (next * plane_normal) - plane_offset;

        bool current_inside = current_dist <= 0;
        bool next_inside = next_dist <= 0;

        if (current_inside)
        {
            output.push_back(current);

            if (!next_inside)
            {
                // Exiting: add intersection point
                linkit::real t = current_dist / (current_dist - next_dist);
                linkit::Vector3 intersection = current + (next - current) * t;
                output.push_back(intersection);
            }
        }
        else if (next_inside)
        {
            // Entering: add intersection point
            linkit::real t = current_dist / (current_dist - next_dist);
            linkit::Vector3 intersection = current + (next - current) * t;
            output.push_back(intersection);
        }
    }

    return output;
}

void CollisionHandler::generate_face_contacts(
    ColliderBox& reference_box,
    ColliderBox& incident_box,
    const BoxAxes& ref_axes,
    const BoxAxes& inc_axes,
    int reference_face_index,
    bool flip_normal,
    linkit::real penetration,
    CollisionData& collision_data)
{
    // Get the reference face normal (pointing outward from reference box)
    linkit::Vector3 ref_normal = ref_axes.axes[reference_face_index];
    linkit::Vector3 to_incident = inc_axes.center - ref_axes.center;

    // Make sure normal points toward incident box
    if (ref_normal * to_incident < 0)
    {
        ref_normal = ref_normal * -1.0f;
    }

    // Find the incident face (most anti-parallel to reference normal)
    int incident_face_index = 0;
    linkit::real min_dot = 1e20f;

    for (int i = 0; i < 3; ++i)
    {
        linkit::real dot = ref_normal * inc_axes.axes[i];
        if (dot < min_dot)
        {
            min_dot = dot;
            incident_face_index = i;
        }
        // Also check negative direction
        if (-dot < min_dot)
        {
            min_dot = -dot;
            incident_face_index = i;
        }
    }

    // Get incident face vertices
    // Determine which side of the incident box to use
    linkit::real dot_check = ref_normal * inc_axes.axes[incident_face_index];
    std::vector<linkit::Vector3> incident_verts;

    const linkit::Vector3& inc_half = incident_box.half_sizes;
    linkit::real inc_half_sizes[3] = {inc_half.x, inc_half.y, inc_half.z};

    int axis1 = (incident_face_index + 1) % 3;
    int axis2 = (incident_face_index + 2) % 3;

    linkit::Vector3 face_offset = inc_axes.axes[incident_face_index] * inc_half_sizes[incident_face_index];
    if (dot_check > 0)
    {
        face_offset = face_offset * -1.0f;
    }

    linkit::Vector3 face_center = inc_axes.center + face_offset;

    incident_verts.resize(4);
    incident_verts[0] = face_center + inc_axes.axes[axis1] * inc_half_sizes[axis1] + inc_axes.axes[axis2] * inc_half_sizes[axis2];
    incident_verts[1] = face_center - inc_axes.axes[axis1] * inc_half_sizes[axis1] + inc_axes.axes[axis2] * inc_half_sizes[axis2];
    incident_verts[2] = face_center - inc_axes.axes[axis1] * inc_half_sizes[axis1] - inc_axes.axes[axis2] * inc_half_sizes[axis2];
    incident_verts[3] = face_center + inc_axes.axes[axis1] * inc_half_sizes[axis1] - inc_axes.axes[axis2] * inc_half_sizes[axis2];

    // Clip incident face against the 4 side planes of the reference face
    const linkit::Vector3& ref_half = reference_box.half_sizes;
    linkit::real ref_half_sizes[3] = {ref_half.x, ref_half.y, ref_half.z};

    int ref_axis1 = (reference_face_index + 1) % 3;
    int ref_axis2 = (reference_face_index + 2) % 3;

    // Clip against the 4 side planes
    std::vector<linkit::Vector3> clipped = incident_verts;

    // Plane 1: +ref_axis1
    linkit::real plane_offset = (ref_axes.center * ref_axes.axes[ref_axis1]) + ref_half_sizes[ref_axis1];
    clipped = clip_polygon_against_plane(clipped, ref_axes.axes[ref_axis1], plane_offset);

    // Plane 2: -ref_axis1
    plane_offset = -(ref_axes.center * ref_axes.axes[ref_axis1]) + ref_half_sizes[ref_axis1];
    clipped = clip_polygon_against_plane(clipped, ref_axes.axes[ref_axis1] * -1.0f, plane_offset);

    // Plane 3: +ref_axis2
    plane_offset = (ref_axes.center * ref_axes.axes[ref_axis2]) + ref_half_sizes[ref_axis2];
    clipped = clip_polygon_against_plane(clipped, ref_axes.axes[ref_axis2], plane_offset);

    // Plane 4: -ref_axis2
    plane_offset = -(ref_axes.center * ref_axes.axes[ref_axis2]) + ref_half_sizes[ref_axis2];
    clipped = clip_polygon_against_plane(clipped, ref_axes.axes[ref_axis2] * -1.0f, plane_offset);

    if (clipped.empty()) return;


    // Reference face plane offset
    linkit::real ref_face_offset = (ref_axes.center * ref_normal) + ref_half_sizes[reference_face_index];
    if (ref_normal * to_incident < 0)
    {
        ref_face_offset = (ref_axes.center * ref_normal) - ref_half_sizes[reference_face_index];
    }

    // Determine the correct contact normal direction
    linkit::Vector3 contact_normal = flip_normal ? (ref_normal * -1.0f) : ref_normal;

    // Create contacts for each clipped vertex that penetrates
    ContactFeature feature;
    feature.type = ContactFeatureType::FACE_FACE;
    feature.body_a_feature = reference_face_index;
    feature.body_b_feature = incident_face_index;

    for (const auto& point : clipped)
    {
        // Calculate penetration depth for this point
        linkit::real depth = ref_face_offset - (point * ref_normal);

        if (depth > 0)
        {
            // Move the contact point to the reference face
            linkit::Vector3 contact_point = point + ref_normal * depth * 0.5f;

            CollisionContact contact(contact_point, contact_normal, depth, feature);
            collision_data.add_contact(contact);
        }
    }
}

linkit::Vector3 CollisionHandler::closest_point_on_edge(
    const linkit::Vector3& edge_start,
    const linkit::Vector3& edge_dir,
    const linkit::Vector3& point)
{
    linkit::Vector3 to_point = point - edge_start;
    linkit::real edge_length_sq = edge_dir.magnitude_squared();

    if (edge_length_sq < 1e-10)
    {
        return edge_start;
    }

    linkit::real t = (to_point * edge_dir) / edge_length_sq;
    t = std::clamp(t, static_cast<linkit::real>(0.0), static_cast<linkit::real>(1.0));

    return edge_start + edge_dir * t;
}

void CollisionHandler::generate_edge_edge_contact(
    ColliderBox& first,
    ColliderBox& second,
    const BoxAxes& axes1,
    const BoxAxes& axes2,
    int edge_index_1,
    int edge_index_2,
    const linkit::Vector3& best_axis,
    linkit::real penetration,
    CollisionData& collision_data)
{
    // Get edge directions
    linkit::Vector3 edge_dir_1 = axes1.axes[edge_index_1];
    linkit::Vector3 edge_dir_2 = axes2.axes[edge_index_2];

    // Get half sizes
    const linkit::Vector3& half1 = first.half_sizes;
    const linkit::Vector3& half2 = second.half_sizes;
    linkit::real half1_arr[3] = {half1.x, half1.y, half1.z};
    linkit::real half2_arr[3] = {half2.x, half2.y, half2.z};

    // Find a point on each edge
    // The edge is along axis[edge_index], so the other two axes define the position
    int other1_a = (edge_index_1 + 1) % 3;
    int other1_b = (edge_index_1 + 2) % 3;
    int other2_a = (edge_index_2 + 1) % 3;
    int other2_b = (edge_index_2 + 2) % 3;

    // Choose the closest corner on each box
    linkit::Vector3 to_center = axes2.center - axes1.center;

    // For box 1, pick corner based on direction to box 2
    linkit::real sign1_a = (to_center * axes1.axes[other1_a]) > 0 ? 1.0f : -1.0f;
    linkit::real sign1_b = (to_center * axes1.axes[other1_b]) > 0 ? 1.0f : -1.0f;

    linkit::Vector3 edge1_mid = axes1.center +
                                axes1.axes[other1_a] * (half1_arr[other1_a] * sign1_a) +
                                axes1.axes[other1_b] * (half1_arr[other1_b] * sign1_b);

    // For box 2, pick corner based on direction from box 1
    linkit::real sign2_a = (to_center * axes2.axes[other2_a]) < 0 ? 1.0f : -1.0f;
    linkit::real sign2_b = (to_center * axes2.axes[other2_b]) < 0 ? 1.0f : -1.0f;

    linkit::Vector3 edge2_mid = axes2.center +
                                axes2.axes[other2_a] * (half2_arr[other2_a] * sign2_a) +
                                axes2.axes[other2_b] * (half2_arr[other2_b] * sign2_b);

    // Find the closest points between the two edge lines
    // Edge 1: P1 = edge1_mid + s * edge_dir_1, s in [-half1[edge_index_1], half1[edge_index_1]]
    // Edge 2: P2 = edge2_mid + t * edge_dir_2, t in [-half2[edge_index_2], half2[edge_index_2]]

    linkit::Vector3 d = edge1_mid - edge2_mid;
    linkit::real a = edge_dir_1 * edge_dir_1;  // Always >= 0
    linkit::real b = edge_dir_1 * edge_dir_2;
    linkit::real c = edge_dir_2 * edge_dir_2;  // Always >= 0
    linkit::real e = edge_dir_1 * d;
    linkit::real f = edge_dir_2 * d;

    linkit::real denom = a * c - b * b;

    linkit::real s, t;

    if (std::abs(denom) < 1e-10f)
    {
        // Edges are nearly parallel - this shouldn't happen often due to SAT checks
        s = 0;
        t = f / c;
    }
    else
    {
        s = (b * f - c * e) / denom;
        t = (a * f - b * e) / denom;
    }

    // Clamp to edge bounds
    s = std::clamp(s, -half1_arr[edge_index_1], half1_arr[edge_index_1]);
    t = std::clamp(t, -half2_arr[edge_index_2], half2_arr[edge_index_2]);

    // Calculate the closest points
    linkit::Vector3 point1 = edge1_mid + edge_dir_1 * s;
    linkit::Vector3 point2 = edge2_mid + edge_dir_2 * t;

    // Contact point is the midpoint
    linkit::Vector3 contact_point = (point1 + point2) * 0.5f;

    // Ensure normal points from first to second
    linkit::Vector3 contact_normal = best_axis;
    if (contact_normal * to_center < 0)
    {
        contact_normal = contact_normal * -1.0f;
    }

    ContactFeature feature;
    feature.type = ContactFeatureType::EDGE_EDGE;
    feature.body_a_feature = edge_index_1;
    feature.body_b_feature = edge_index_2;

    CollisionContact contact(contact_point, contact_normal, penetration, feature);
    collision_data.add_contact(contact);
}

