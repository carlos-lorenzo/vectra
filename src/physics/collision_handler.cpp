#include "vectra/physics/collision_handler.h"
#include <cmath>
#include <iostream>
#include <ostream>
#include <unistd.h>

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
                contact.relative_positions = {
                    objects[0]->rb.transform.position - contact.collision_point,
                    objects[1]->rb.transform.position - contact.collision_point
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
    const Transform& tf1 = first.get_transform();
    const Transform& tf2 = second.get_transform();

    linkit::Vector3 p1 = tf1.position;
    linkit::Vector3 p2 = tf2.position;
    linkit::Vector3 to_center = p2 - p1;

    // Helper to extract axes from transform
    auto get_axes = [](const Transform& tf, const linkit::Vector3& center) {
        auto model = tf.get_model_matrix();
        linkit::Vector3 ax = model * linkit::Vector3(1, 0, 0) - center;
        linkit::Vector3 ay = model * linkit::Vector3(0, 1, 0) - center;
        linkit::Vector3 az = model * linkit::Vector3(0, 0, 1) - center;
        ax.normalize(); ay.normalize(); az.normalize();
        return std::array<linkit::Vector3, 3>{ax, ay, az};
    };

    auto axes1 = get_axes(tf1, p1);
    auto axes2 = get_axes(tf2, p2);

    linkit::real min_overlap = 1e20; // Initialize with a large value
    linkit::Vector3 best_axis;

    // Helper to test a separating axis
    // Returns false if separated (no collision), true otherwise
    auto test_axis = [&](const linkit::Vector3& axis) -> bool {
        if (axis.magnitude_squared() < 1e-6) return true; // Ignore zero-length axes (parallel edges)

        linkit::Vector3 n = axis;
        n.normalize();

        // Project half-sizes of box 1 onto normal
        linkit::real r1 = first.half_sizes.x * std::abs(axes1[0] * n) +
                          first.half_sizes.y * std::abs(axes1[1] * n) +
                          first.half_sizes.z * std::abs(axes1[2] * n);

        // Project half-sizes of box 2 onto normal
        linkit::real r2 = second.half_sizes.x * std::abs(axes2[0] * n) +
                          second.half_sizes.y * std::abs(axes2[1] * n) +
                          second.half_sizes.z * std::abs(axes2[2] * n);

        linkit::real dist = std::abs(to_center * n);
        linkit::real overlap = (r1 + r2) - dist;

        if (overlap < 0) return false; // Separating axis found

        if (overlap < min_overlap) {
            min_overlap = overlap;
            best_axis = n;
        }
        return true;
    };

    // 1. Test face normals of box 1
    for (const auto& axis : axes1) if (!test_axis(axis)) return collision_data;

    // 2. Test face normals of box 2
    for (const auto& axis : axes2) if (!test_axis(axis)) return collision_data;

    // 3. Test cross-products of edges
    auto cross = [](const linkit::Vector3& a, const linkit::Vector3& b) {
        return linkit::Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    };

    for (const auto& a1 : axes1) {
        for (const auto& a2 : axes2) {
            if (!test_axis(cross(a1, a2))) return collision_data;
        }
    }

    // If we get here, there is a collision.
    // Ensure normal points from 1 to 2
    if (best_axis * to_center < 0) {
        best_axis = best_axis * -1.0f;
    }

    // Calculate support points to estimate contact point
    auto get_support = [](const ColliderBox& box, const std::array<linkit::Vector3, 3>& axes, const linkit::Vector3& dir) {
        linkit::Vector3 support = box.get_transform().position;
        linkit::real sign_x = (dir * axes[0]) >= 0 ? 1.0f : -1.0f;
        linkit::real sign_y = (dir * axes[1]) >= 0 ? 1.0f : -1.0f;
        linkit::real sign_z = (dir * axes[2]) >= 0 ? 1.0f : -1.0f;

        return support + axes[0] * (box.half_sizes.x * sign_x) +
                         axes[1] * (box.half_sizes.y * sign_y) +
                         axes[2] * (box.half_sizes.z * sign_z);
    };

    linkit::Vector3 pt1 = get_support(first, axes1, best_axis);
    linkit::Vector3 pt2 = get_support(second, axes2, best_axis * -1.0f);

    // Approximate contact point as the average of the deepest points
    linkit::Vector3 contact_point = (pt1 + pt2) * 0.5f;

    CollisionContact contact(contact_point, best_axis, min_overlap);
    collision_data.add_contact(contact);
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
            linkit::Vector3 current_velocity = linkit::Vector3(0, 0, 0);
            for (int i=0; i<2;i++)
            {

                linkit::Vector3 torque = contact.relative_positions[i] % contact.collision_normal;
                linkit::Vector3 delta_angular_velocity = collision.objects[i]->rb._local_inverse_inertia_tensor * torque;
                delta_velocity += (delta_angular_velocity % contact.relative_positions[i]) * contact.collision_normal;
                delta_velocity += collision.objects[i]->rb.inverse_mass;
                current_velocity += collision.objects[i]->rb.angular_velocity % contact.relative_positions[i] + collision.objects[i]->rb.velocity;
            }

            linkit::Vector3 contact_velocity = contact.contact_basis_to_world_inverse() * current_velocity;
            linkit::real desired_delta_velocity = -contact_velocity.x * (1 + collision.restitution);
            linkit::Vector3 impulse_contact;
            impulse_contact.x = desired_delta_velocity / delta_velocity;
            linkit::Vector3 impulse = contact.contact_basis_to_world() * impulse_contact;

            for (int i=0; i<2;i++)
            {

                linkit::Vector3 velocity_change = impulse * collision.objects[i]->rb.inverse_mass;
                collision.objects[i]->rb.velocity += velocity_change;

                linkit::Vector3 impulse_torque = impulse % contact.relative_positions[i];
                linkit::Vector3 rotation_change = collision.objects[i]->rb._local_inverse_inertia_tensor * impulse_torque;
                collision.objects[i]->rb.angular_velocity += rotation_change;

                impulse *= -1;
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
                linkit::Matrix3 inverse_inertia_tensor = body->rb._local_inverse_inertia_tensor;
                linkit::Vector3 impulsive_torque = contact.relative_positions[i] % contact.collision_normal;
                linkit::Vector3 impulse_per_move = inverse_inertia_tensor * impulsive_torque;
                linkit::Vector3 rotation_per_move = impulse_per_move * 1/angular_inertia_contact[0];
                linkit::Vector3 rotation = rotation_per_move * angular_move[i];
                //body->rb.transform.rotation.add_scaled_vector(rotation, 1);

                // Linear resolution
                body->rb.transform.translate(contact.collision_normal * linear_move[i]);


            }
        }
    }

}

void CollisionHandler::clear_contacts()
{
    collisions.clear();
}
