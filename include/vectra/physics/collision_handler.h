#ifndef VECTRA_COLLISION_HANDLER_H
#define VECTRA_COLLISION_HANDLER_H

#include <vector>
#include <array>

#include "vectra/physics/BVHNode.h"
#include "vectra/physics/collision_contact.h"
#include "vectra/physics/collision_data.h"

// Collider imports - handler will implement collision checking between every possible pair
#include "vectra/physics/colliders/collider_sphere.h"
#include "vectra/physics/colliders/collider_box.h"

class CollisionHandler
{
    public:
        CollisionHandler();

        void add_collision(const CollisionData& collision);
        void narrow_phase(const std::vector<PotentialContact>& potential_contacts);

        CollisionData solve_collision(ColliderPrimitive& first, ColliderPrimitive& second);
        static CollisionData solve_sphere_sphere(const ColliderSphere& first, const ColliderSphere& second);
        static CollisionData solve_box_box(ColliderBox& first, ColliderBox& second);
        static CollisionData solve_sphere_box(ColliderSphere& sphere, ColliderBox& box);
        void solve_contacts();
        void resolve_interpretations();
        void clear_contacts();

        std::vector<CollisionData> collisions;

    private:

        // Helper structures for box-box collision
        struct BoxAxes
        {
            std::array<linkit::Vector3, 3> axes;
            linkit::Vector3 center;
        };

        // Multi-point contact generation helpers
        static BoxAxes get_box_axes(const Transform& tf);
        static void generate_face_contacts(
            ColliderBox& reference_box,
            ColliderBox& incident_box,
            const BoxAxes& ref_axes,
            const BoxAxes& inc_axes,
            int reference_face_index,
            bool flip_normal,
            linkit::real penetration,
            CollisionData& collision_data
        );
        static void generate_edge_edge_contact(
            ColliderBox& first,
            ColliderBox& second,
            const BoxAxes& axes1,
            const BoxAxes& axes2,
            int edge_index_1,
            int edge_index_2,
            const linkit::Vector3& best_axis,
            linkit::real penetration,
            CollisionData& collision_data
        );
        static std::vector<linkit::Vector3> get_face_vertices(
            const ColliderBox& box,
            const BoxAxes& axes,
            int face_index
        );
        static std::vector<linkit::Vector3> clip_polygon_against_plane(
            const std::vector<linkit::Vector3>& polygon,
            const linkit::Vector3& plane_normal,
            linkit::real plane_offset
        );
        static linkit::Vector3 closest_point_on_edge(
            const linkit::Vector3& edge_start,
            const linkit::Vector3& edge_dir,
            const linkit::Vector3& point
        );
};

#endif //VECTRA_COLLISION_HANDLER_H
