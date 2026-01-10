#ifndef VECTRA_ENGINE_H
#define VECTRA_ENGINE_H



#include "vectra/core/engine_state.h"
#include "vectra/core/scene.h"

#include "vectra/rendering/renderer.h"
#include "vectra/rendering/engine_ui.h"

class Engine
{
private:
    EngineState state_;

    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<EngineUI> ui;

public:
    Engine();
    void load_scene();
    void run();
};
#endif //VECTRA_ENGINE_H