#include "vectra/physics/bounding_volumes/bounding_sphere.h"
#include "vectra/core/gameobject.h"


BoundingSphere::BoundingSphere(const linkit::Vector3& center, linkit::real radius) :
center(center),
radius(radius) {}

BoundingSphere::BoundingSphere(const BoundingSphere& first, const BoundingSphere& second)
{
    linkit::Vector3 offset = second.center - first.center;
    linkit::real distance_squared = offset * offset;
    linkit::real radius_diff = first.radius - second.radius;

    if (radius_diff * radius_diff >= distance_squared)
    {
        if (first.radius > second.radius)
        {
            center = first.center;
            radius = first.radius;
        }
        else
        {
            center = second.center;
            radius = second.radius;
        }
    }
    else
    {
        linkit::real distance = linkit::real_sqrt(distance_squared);
        radius = (first.radius + second.radius + distance) / 2;
        center = first.center;
        if (distance > 0)
        {
            center += offset * (radius - first.radius) / distance;
        }
    }

}

void BoundingSphere::update_position(const GameObject& obj)
{
    center = obj.rb.transform.position;
}


linkit::real BoundingSphere::expected_growth(const BoundingSphere& other) const
{
    return BoundingSphere(*this, other).size() - size();
}

bool BoundingSphere::overlaps(const BoundingSphere& other) const
{
    linkit::Vector3 offset = other.center - center;
    linkit::real distance_squared = offset*offset;
    return distance_squared <= (radius + other.radius) * (radius + other.radius);
}

linkit::real BoundingSphere::size() const
{
    return static_cast<linkit::real>(4.0 / 3.0) * linkit::PI * radius * radius * radius;
}