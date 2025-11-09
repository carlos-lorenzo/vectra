#ifndef VECTRA_BOUNDING_SPHERE_H
#define VECTRA_BOUNDING_SPHERE_H
#include "linkit/linkit.h"
#include "vectra/rendering/model.h"
struct BoundingSphere
{
    linkit::Vector3 center;
    linkit::real radius;



public:
    BoundingSphere(const linkit::Vector3& center, linkit::real radius);
    BoundingSphere(const BoundingSphere &first, const BoundingSphere &second);
    [[nodiscard]] linkit::real expected_growth(const BoundingSphere &other) const;
    [[nodiscard]] bool overlaps(const BoundingSphere &other) const;
    [[nodiscard]] linkit::real size() const; // make const
};

#endif //VECTRA_BOUNDING_SPHERE_H