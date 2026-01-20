#ifndef VECTRA_ENGINE_UI_H
#define VECTRA_ENGINE_UI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Forward declaration of ImGui types to avoid forcing all includes to have imgui.h
struct ImVec4;

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
    std::string upload_status_message_;  // Status message for file upload

    // Color palette (Dracula-inspired) stored as class attributes
    static ImVec4 color_background;
    static ImVec4 color_current_line;
    static ImVec4 color_foreground;
    static ImVec4 color_comment;
    static ImVec4 color_purple;
    static ImVec4 color_green;
    static ImVec4 color_pink;
    static ImVec4 color_cyan;
    static ImVec4 color_red;
    static ImVec4 color_orange;
    static ImVec4 color_yellow;
    static ImVec4 color_popup_bg;
    static ImVec4 color_modal_dim;

    static void apply_dracula_theme();
    void setup_initial_dock_layout(ImGuiID dockspace_id);
    void draw_toolbar(EngineState& state);
    void draw_hierarchy(const SceneSnapshot& scene_snapshot);
    void draw_inspector(const SceneSnapshot& scene_snapshot);
    void draw_scene_view(EngineState& state, GLuint scene_texture_id);
    void draw_scene_selection(EngineState& state);
    static void draw_restart_overlay();
};
#endif //VECTRA_ENGINE_UI_H
