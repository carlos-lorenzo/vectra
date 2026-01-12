#ifndef VECTRA_ENGINE_STATE_H
#define VECTRA_ENGINE_STATE_H

#include "linkit/linkit.h"

struct EngineState
{
    bool is_paused = false;
    bool is_running = true;
    bool scene_view_focused = false; // True when Scene View window is focused and hovered

    linkit::real simulation_speed = 1.0f; // 1.0 = normal speed, 0.5 = half-speed, 2.0 = double speed
    linkit::real target_fps = 300.0f; // Target frames per second for rendering
    linkit::real simulation_frequency = 60.0f; // Physics update frequency in Hz

    int max_collision_contacts = 100; // Max number of collision contacts to consider per physics update
    int window_width = 1920;
    int window_height = 1080;

    // Scene view framebuffer size (updated by UI)
    int scene_view_width = 1280;
    int scene_view_height = 720;
};

#endif //VECTRA_ENGINE_STATE_H