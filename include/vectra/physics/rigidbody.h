//
// Created by carlos on 9/29/25.
//

#ifndef VECTRA_RIGIDBODY_H
#define VECTRA_RIGIDBODY_H
#include "transform.h"
#include "linkit/linkit.h"


class Rigidbody
{
    public:
        bool has_moved;
        Transform transform;
        linkit::Vector3 velocity;
        linkit::Vector3 acceleration;
        linkit::Vector3 last_frame_acceleration;

        linkit::Vector3 angular_velocity;
        linkit::Vector3 angular_acceleration;

        linkit::Vector3 accumulated_force;
        linkit::Vector3 accumulated_torque;

        linkit::real mass;
        linkit::real inverse_mass;
        linkit::Matrix3 _local_inverse_inertia_tensor;

        linkit::real linear_damping;
        linkit::real last_dt;

        // Sleep parameters
        linkit::real motion;
        bool is_awake;
        bool can_sleep;


        Rigidbody();
        void clear_accumulators();

        linkit::Matrix4 transform_matrix();
        linkit::Matrix4 inverse_transform_matrix();
        linkit::Vector3 local_to_world(const linkit::Vector3& local_point);
        linkit::Vector3 world_to_local(const linkit::Vector3& world_point);
        [[nodiscard]] linkit::Matrix3 get_inverse_inertia_tensor() const;

        void set_awake(bool awake = true);
        [[nodiscard]] bool get_awake() const;
        void set_can_sleep(bool can_sleep = true);
        [[nodiscard]] bool get_can_sleep() const;

        void add_force(const linkit::Vector3& force);
        // Will add a torque based on force applied at a point in world space
        void add_force_at_world_point(const linkit::Vector3& force, const linkit::Vector3& point);
        void add_force_at_local_point(const linkit::Vector3& force, const linkit::Vector3& point);
        void compute_accelerations();
        void step_rotation(linkit::real dt);
        void step_position(linkit::real dt);
        void step(linkit::real dt);
        [[nodiscard]] bool has_finite_mass() const;
        [[nodiscard]] bool has_infinite_mass() const;
        [[nodiscard]] linkit::Matrix3 cuboid_inertia_tensor() const;
        [[nodiscard]] linkit::Matrix3 sphere_inertia_tensor() const;
        void set_inverse_inertia_tensor(const linkit::Matrix3& inertia_tensor);
};


#endif //VECTRA_RIGIDBODY_H

