#ifndef VECTRA_ENGINE_STATE_H
#define VECTRA_ENGINE_STATE_H

#include "linkit/linkit.h"

struct EngineState
{
    bool is_paused = false;

    linkit::real simulation_speed = 1.0f; // 1.0 = normal speed, 0.5 = half-speed, 2.0 = double speed
    linkit::real target_fps = 144.0f; // Target frames per second for rendering
    linkit::real simulation_frequency = 60.0f; // Physics update frequency in Hz

    int window_width = 1920;
    int window_height = 1080;
};

#endif //VECTRA_ENGINE_STATE_H