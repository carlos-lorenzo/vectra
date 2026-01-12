#ifndef VECTRA_ENGINE_UI_H
#define VECTRA_ENGINE_UI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "vectra/core/engine_state.h"
#include "vectra/core/scene_snapshot.h"

// Forward declaration for ImGuiID
using ImGuiID = unsigned int;

class EngineUI
{
public:
    static void initialize(GLFWwindow* window);
    void draw(EngineState& state, SceneSnapshot& scene_snapshot, GLuint scene_texture_id);
    void end_frame();
    void cleanup();

private:
    int selected_object_index_ = -1;
    bool first_frame_ = true;  // For initial dock layout setup

    static void apply_dracula_theme();
    void setup_initial_dock_layout(ImGuiID dockspace_id);
    void draw_toolbar(EngineState& state);
    void draw_hierarchy(const SceneSnapshot& scene_snapshot);
    void draw_inspector(const SceneSnapshot& scene_snapshot);
    void draw_scene_view(EngineState& state, GLuint scene_texture_id);
};
#endif //VECTRA_ENGINE_UI_H
