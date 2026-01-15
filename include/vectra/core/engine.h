#ifndef VECTRA_ENGINE_H
#define VECTRA_ENGINE_H



#include "vectra/core/engine_state.h"
#include "vectra/core/scene.h"
#include "vectra/core/render_queue.h"
#include "vectra/core/scene_snapshot.h"
#include "vectra/core/scene_serializer.h"

#include "vectra/rendering/renderer.h"
#include "vectra/rendering/engine_ui.h"
#include "vectra/rendering/camera.h"

class Engine
{
private:
    EngineState state_;
    RenderQueue<SceneSnapshot> render_queue_;
    SceneSerializer serializer_;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<EngineUI> ui;


public:
    std::unique_ptr<Scene> scene;
    Engine();
    void load_scene(const std::string& filename = "default_scene.json");
    void run_single_thread();
    void physics_thread_func();
    void rendering_thread_func();
    void run();
};
#endif //VECTRA_ENGINE_H