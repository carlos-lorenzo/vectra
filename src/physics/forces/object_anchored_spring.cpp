#include "vectra/physics/forces/object_anchored_spring.h"

/**
 *
 * @param anchor_object
 * @param spring_constant
 * @param rest_length
 * @param damping
 */
ObjectAnchoredSpring::ObjectAnchoredSpring(GameObject* anchor_object, const linkit::real spring_constant,
                                           const linkit::real rest_length, const linkit::real damping)
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

linkit::Vector3 ObjectAnchoredSpring::get_anchor_point() const
{
    return anchor_object->rb.transform.position;
}
