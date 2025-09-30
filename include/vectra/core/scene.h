#ifndef VECTRA_SCENE_H
#define VECTRA_SCENE_H


#include <vector>

#include "vectra/core/gameobject.h"

class Scene
{
    public:
        Scene();
        std::vector<GameObject> game_objects;
        void add_game_object(const GameObject& obj);

};
#endif //VECTRA_SCENE_H