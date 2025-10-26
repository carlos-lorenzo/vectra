#ifndef VECTRA_SCENE_H
#define VECTRA_SCENE_H


#include <vector>

#include "vectra/rendering/camera.h"
#include "vectra/core/gameobject.h"

class Scene
{
    public:
        Camera camera;
        Scene();
        std::vector<GameObject> game_objects;
        void add_game_object(const GameObject& obj);
        void step(linkit::real dt);

};
#endif //VECTRA_SCENE_H