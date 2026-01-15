#ifndef VECTRA_ENGINE_STATE_H
#define VECTRA_ENGINE_STATE_H

#include "linkit/linkit.h"

struct EngineState
{
    bool is_paused = true;
    bool is_running = true;
    bool scene_view_focused = false; // True when the Scene View window is focused and hovered
    bool enable_vsync = true;

    linkit::real simulation_speed = 1.0; // 1.0 = normal speed, 0.5 = half-speed, 2.0 = double speed
    linkit::real target_fps = 144.0; // Target frames per second for rendering
    linkit::real simulation_frequency = 144.0; // Physics update frequency in Hz

    int max_collision_contacts = 500; // Max number of collision contacts to consider per physics update
    int window_width = 2560;
    int window_height = 1440;

    // Scene view framebuffer size (updated by UI)
    int scene_view_width = 1920;
    int scene_view_height = 1080;

    // Collision resolution parameters
    unsigned int position_iterations = 8;    // Max iterations for penetration resolution
    unsigned int velocity_iterations = 8;    // Max iterations for velocity resolution
    linkit::real position_epsilon = 0.001f;  // Minimum penetration to resolve
    linkit::real velocity_epsilon = 0.001f;  // Minimum closing velocity to resolve

    std::string loaded_scene = "default_scene.json"; // Last loaded scene filename -> needed for restart functionality
    bool scene_should_restart = false; // Flag to indicate if the scene should be restarted
};

#endif //VECTRA_ENGINE_STATE_H