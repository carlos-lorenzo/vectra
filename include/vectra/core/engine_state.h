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

    int max_collision_contacts = 1000; // Max number of collision contacts to consider per physics update


    int window_width = 2560;
    int window_height = 1440;

    // Scene view framebuffer size (updated by UI)
    int scene_view_width = 1920;
    int scene_view_height = 1080;




    std::string loaded_scene = "default_scene.json"; // Last loaded scene filename -> needed for restart functionality
    std::string requested_scene_file; // If non-empty, the engine will load this scene at the next opportunity
    bool scene_should_restart = false; // Flag to indicate if the scene should be restarted

    // Draw debug info
    bool draw_forces = false;
    bool draw_bvh = false;

    // Shadow settings
    bool draw_shadows = true;                  // Global toggle to enable/disable shadow generation
    int shadow_resolution_default = 2048;     // Default resolution for shadow maps
    int shadow_pcf_kernel = 3;                // PCF kernel size (3 -> 3x3 samples), 0 = off
    float shadow_bias = 0.005f;               // Default bias to reduce shadow acne
};

#endif //VECTRA_ENGINE_STATE_H