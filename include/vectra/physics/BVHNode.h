#pragma once
#ifndef VECTRA_BVHNODE_H
#define VECTRA_BVHNODE_H

#include <memory>
#include "vectra/core/gameobject.h"

struct PotentialContact
{
    GameObject* objects[2];
};

template <class BoundingVolumeClass>
class BVHNode
{
public:
    BVHNode* parent{nullptr};
    BVHNode* children[2]{nullptr, nullptr};
    std::unique_ptr<BoundingVolumeClass> bounding_volume;
    GameObject* object{nullptr};

    BVHNode() = default;

    // Leaf constructor
    BVHNode(BVHNode* p, const BoundingVolumeClass& volume, GameObject* obj)
        : parent(p), bounding_volume(std::make_unique<BoundingVolumeClass>(volume)), object(obj) {}

    ~BVHNode() {
        delete children[0];
        delete children[1];
    }

    bool is_leaf() const { return object != nullptr; }

    void recalculate_bounding_volume()
    {
        if (!children[0] || !children[1]) return;
        bounding_volume = std::make_unique<BoundingVolumeClass>(
            *children[0]->bounding_volume,
            *children[1]->bounding_volume
        );
    }

    BVHNode* insert(GameObject* new_obj, const BoundingVolumeClass& new_volume)
    {
        if (is_leaf())
        {
            children[0] = new BVHNode(this, *bounding_volume, object);
            children[1] = new BVHNode(this, new_volume, new_obj);
            object = nullptr;
        }
        else
        {
            if (children[0]->bounding_volume->expected_growth(new_volume) <
                children[1]->bounding_volume->expected_growth(new_volume))
            {
                children[0]->insert(new_obj, new_volume);
            }
            else
            {
                children[1]->insert(new_obj, new_volume);
            }
        }
        recalculate_bounding_volume();
        return this;

    }

    bool overlaps(const BVHNode<BoundingVolumeClass>* other) const
    {
        return bounding_volume->overlaps(*other->bounding_volume);
    }

    unsigned int potential_contacts(PotentialContact* contacts, unsigned int limit)
    {
        if (is_leaf() || limit == 0) return 0;
        return children[0]->potential_contacts_with(children[1], contacts, limit);
    }

    unsigned int potential_contacts_with(BVHNode<BoundingVolumeClass>* other,
                                         PotentialContact* contacts,
                                         unsigned int limit)
    {
        if (limit == 0 || !overlaps(other)) return 0;

        if (is_leaf() && other->is_leaf())
        {
            contacts->objects[0] = object;
            contacts->objects[1] = other->object;
            return 1;
        }

        unsigned int count = 0;

        if (other->is_leaf() || (!is_leaf() && bounding_volume->size() >= other->bounding_volume->size()))
        {
            count = children[0]->potential_contacts_with(other, contacts, limit);
            if (count < limit)
            {
                return count + children[1]->potential_contacts_with(other, contacts + count, limit - count);
            }
            return count;
        }
        else
        {
            count = potential_contacts_with(other->children[0], contacts, limit);
            if (count < limit)
            {
                return count + potential_contacts_with(other->children[1], contacts + count, limit - count);
            }
            return count;
        }
    }

    // Recompute bounds from this node up to the root.
    void recalc_upwards() {
        BVHNode* n = this;
        while (n) {
            if (!n->is_leaf() && n->children[0] && n->children[1]) {
                n->recalculate_bounding_volume();
            }
            n = n->parent;
        }
    }

    // Remove this leaf from the BVH.
    // Precondition: this node is a leaf (object != nullptr).
    void remove() {
        // Root leaf: just clear.
        if (!parent) {
            object = nullptr;
            bounding_volume.reset();
            return;
        }

        // Must be a leaf for this operation.
        // If you need to remove internal nodes, handle that separately.
        // (Early out if not a leaf to avoid corrupting the tree.)
        if (!is_leaf()) {
            // Optional: define semantics for removing internal nodes.
            return;
        }

        BVHNode* p = parent;
        BVHNode* sibling = (p->children[0] == this) ? p->children[1] : p->children[0];

        // Promote sibling into parent.
        p->object = sibling->object;
        p->bounding_volume = std::move(sibling->bounding_volume);
        p->children[0] = sibling->children[0];
        p->children[1] = sibling->children[1];
        if (p->children[0]) p->children[0]->parent = p;
        if (p->children[1]) p->children[1]->parent = p;

        // Detach moved children so sibling's destructor won't delete them.
        sibling->children[0] = nullptr;
        sibling->children[1] = nullptr;
        sibling->object = nullptr;

        // Keep a starting point for upward recomputation.
        BVHNode* start = p;

        // Detach self from parent and delete both redundant nodes.
        parent = nullptr; // defensive
        delete sibling;
        delete this;

        // Fix bounding volumes up to the root.
        start->recalc_upwards();
    }
};

#endif // VECTRA_BVHNODE_H