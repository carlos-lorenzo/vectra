#include <iostream>

#include "vectra/core/engine.h"
#include "vectra/core/scene_serializer.h"

int main()
{
    const auto engine = std::make_unique<Engine>();
    engine->load_scene("default_scene.json");
    engine->run();
    return 0;
}

