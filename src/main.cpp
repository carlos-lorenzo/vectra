#include "gameobject.h"
#include "vectra/rendering/renderer.h"
#include "vectra/rendering/light_source.h"
#include "vectra/physics/forces/simple_gravity.h"
#include "vectra/physics/forces/newtonian_gravity.h"
#include "vectra/physics/forces/anchored_spring.h"
#include "linkit/linkit.h"
#include "vectra/core/engine.h"


int main()
{
    const auto engine = new Engine();
    engine->load_scene();
    engine->run();
    delete engine;
    return 0;
}

