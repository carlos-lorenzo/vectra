#include "vectra/physics/forces/object_anchored_spring.h"

ObjectAnchoredSpring::ObjectAnchoredSpring(GameObject* anchor_object, linkit::real spring_constant, linkit::real rest_length, linkit::real damping)
    : anchor_object(anchor_object),
      spring_constant(spring_constant),
      rest_length(rest_length),
      damping(damping)
{
}

void ObjectAnchoredSpring::update_force(GameObject& obj, linkit::real dt)
{
    linkit::Vector3 force = obj.rb.transform.position;
    force -= anchor_object->rb.transform.position;

    linkit::real magnitude = force.magnitude();
    magnitude = (rest_length - magnitude) * spring_constant;

    force.normalize();
    force *= magnitude;
    obj.rb.add_force(force);
}
