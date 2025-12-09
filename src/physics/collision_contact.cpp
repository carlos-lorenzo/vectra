#include "vectra/physics/collision_contact.h"

CollisionContact::CollisionContact(const linkit::Vector3& collision_point, const linkit::Vector3& collision_normal, const linkit::real penetration_depth) :
collision_point(collision_point),
collision_normal(collision_normal),
penetration_depth(penetration_depth) {}