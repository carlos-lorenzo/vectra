#include "vectra/physics/forces/anchored_spring.h"

#include <iostream>
#include <ostream>

AnchoredSpring::AnchoredSpring(const linkit::Vector3& anchor_point, linkit::real spring_constant, linkit::real rest_length, linkit::real damping) :
anchor_point(anchor_point),
spring_constant(spring_constant),
rest_length(rest_length),
damping(damping)
{}

void AnchoredSpring::update_force(GameObject& obj, linkit::real dt)
{
    linkit::Vector3 force = obj.rb.transform.position;
    force -= anchor_point;

    linkit::real magnitude = force.magnitude();
    magnitude = (rest_length - magnitude) * spring_constant;

    force.normalize();
    force *= magnitude;
    linkit::Vector3 local_point = 0.0 * linkit::Vector3(1.0f, 1.0f, 1.0f);
    obj.rb.add_force_at_local_point(force, local_point);
}