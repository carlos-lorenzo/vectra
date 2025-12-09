#ifndef VECTRA_COLLISION_DATA_H
#define VECTRA_COLLISION_DATA_H

#include <vector>
#include "vectra/physics/collision_contact.h"

class CollisionData
{
    public:
        CollisionData();
        void add_contact(const CollisionContact& contact);
        [[nodiscard]] const std::vector<CollisionContact>& get_contacts() const;

    private:
        std::vector<CollisionContact> contacts;
    // Will include other parameters such as restitution once I get narrow phase detection working
};

#endif //VECTRA_COLLISION_DATA_H