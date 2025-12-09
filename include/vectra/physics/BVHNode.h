#pragma once
#ifndef VECTRA_BVHNODE_H
#define VECTRA_BVHNODE_H

#include <memory>
#include <vector>
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

    [[nodiscard]] bool is_leaf() const { return object != nullptr; }

    void recalculate_bounding_volume()
    {
        if (!children[0] || !children[1]) return;
        bounding_volume = std::make_unique<BoundingVolumeClass>(
            *children[0]->bounding_volume,
            *children[1]->bounding_volume
        );
    }

    BVHNode<BoundingVolumeClass>* insert(GameObject* new_obj, const BoundingVolumeClass& new_volume)
    {
        if (is_leaf())
        {
            // Split this leaf into two children
            children[0] = new BVHNode(this, *bounding_volume, object);      // old object
            children[1] = new BVHNode(this, new_volume, new_obj);    // new object
            object = nullptr;

            recalculate_bounding_volume();
            return children[1]; // return the newly inserted leaf
        }

        // Choose a child with the least growth and recurse
        BVHNode* chosen = (children[0]->bounding_volume->expected_growth(new_volume) <
                           children[1]->bounding_volume->expected_growth(new_volume))
                            ? children[0] : children[1];

        BVHNode* leaf = chosen->insert(new_obj, new_volume);
        recalculate_bounding_volume();
        return leaf;
    }

    bool overlaps(const BVHNode<BoundingVolumeClass>* other) const
    {
        return bounding_volume->overlaps(*other->bounding_volume);
    }


    std::vector<PotentialContact> potential_contacts_inside(std::vector<PotentialContact> contacts, unsigned int limit)
    {
        if (contacts.size() >= limit) return contacts;
        if (is_leaf()) return contacts;

        if (children[0] && children[1] && children[0]->overlaps(children[1]))
        {
            contacts = children[0]->potential_contacts_with(children[1], contacts, limit);
            if (contacts.size() >= limit) return contacts;
        }

        if (children[0])
        {
            contacts = children[0]->potential_contacts_inside(contacts, limit);
            if (contacts.size() >= limit) return contacts;
        }
        if (children[1])
        {
            contacts = children[1]->potential_contacts_inside(contacts, limit);
        }
        return contacts;
    }

    std::vector<PotentialContact> potential_contacts_with(const BVHNode<BoundingVolumeClass>* other, std::vector<PotentialContact> contacts, unsigned int limit)
    {
        if (contacts.size() >= limit) return contacts;
        if (!other) return contacts;
        if (!overlaps(other)) return contacts;

        // Both leaves -> record contact
        if (is_leaf() && other->is_leaf())
        {
            contacts.push_back({object, other->object});
            return contacts;
        }

        // Decide which subtree to descend
        if (other->is_leaf() || (!is_leaf() && bounding_volume->size() >= other->bounding_volume->size()))
        {
            if (children[0])
            {
                contacts = children[0]->potential_contacts_with(other, contacts, limit);
                if (contacts.size() >= limit) return contacts;
            }
            if (children[1])
            {
                contacts = children[1]->potential_contacts_with(other, contacts, limit);
            }
            return contacts;
        }
        else
        {
            if (other->children[0])
            {
                contacts = potential_contacts_with(other->children[0], contacts, limit);
                if (contacts.size() >= limit) return contacts;
            }
            if (other->children[1])
            {
                contacts = potential_contacts_with(other->children[1], contacts, limit);
            }
            return contacts;
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

        // Detach moved children so the sibling's destructor won't delete them.
        sibling->children[0] = nullptr;
        sibling->children[1] = nullptr;
        sibling->object = nullptr;

        // Keep a starting point for upward recomputation.
        BVHNode* start = p;

        // Detach self from the parent and delete both redundant nodes.
        parent = nullptr; // defensive
        delete sibling;
        delete this;

        // Fix bounding volumes up to the root.
        start->recalc_upwards();
    }

};

#endif // VECTRA_BVHNODE_H