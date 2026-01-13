#include "vectra/core/engine.h"

int main()
{
    const auto engine = std::make_unique<Engine>();
    engine->load_scene();
    engine->run();
    return 0;
}

