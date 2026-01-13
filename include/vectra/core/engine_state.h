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
    linkit::real simulation_frequency = 60.0; // Physics update frequency in Hz

    int max_collision_contacts = 500; // Max number of collision contacts to consider per physics update
    int window_width = 2560;
    int window_height = 1440;

    // Scene view framebuffer size (updated by UI)
    int scene_view_width = 1920;
    int scene_view_height = 1080;
};

#endif //VECTRA_ENGINE_STATE_H