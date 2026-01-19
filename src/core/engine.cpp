#include <thread>
#include <chrono>
#include <cmath>

#include "linkit/linkit.h"
#include "vectra/core/engine.h"

#include <iostream>
#include <unistd.h>

#include "vectra/core/render_queue.h"


#include "vectra/physics/forces/simple_gravity.h"
#include "vectra/physics/forces/object_anchored_spring.h"


Engine::Engine() : render_queue_()
{
    state_ = EngineState();
    renderer = std::make_unique<Renderer>(&state_);
    scene = std::make_unique<Scene>();

    serializer_ = SceneSerializer();

    ui = std::make_unique<EngineUI>();
    EngineUI::initialize(renderer->get_window());

}

void Engine::load_scene(const std::string& filename)
{
    auto result = serializer_.deserialize_scene(filename);

    // Log errors
    for (const auto& error : result.errors)
    {
        std::cerr << error << std::endl;
    }

    // Abort if there are errors
    if (result.has_errors())
    {
        std::cerr << "Failed to load scene: " << filename << std::endl;
        return;
    }

    // Log warnings
    for (const auto& warning : result.warnings)
    {
        std::cout << warning << std::endl;
    }

    *scene = std::move(result.scene);
    //scene->set_engine_state(state_);
    state_.loaded_scene = filename;
    renderer->setup_from_scene(*scene);
}


void Engine::run_single_thread()
{
    linkit::real t = 0.0;
    const linkit::real dt_phys = 1.0 / state_.simulation_frequency;

    linkit::real currentTime = glfwGetTime();
    linkit::real accumulator = 0.0;

    const linkit::real min_frame_time = 1.0 / state_.target_fps;

    renderer->use_skybox();
    auto window = renderer->get_window();


    while (!glfwWindowShouldClose(window)) {
        // Handle scene restart
        if (state_.scene_should_restart)
        {
            load_scene(state_.loaded_scene);
            state_.scene_should_restart = false;
            accumulator = 0.0;
            currentTime = glfwGetTime();
        }

        double new_time = glfwGetTime();
        double frame_time = new_time - currentTime;
        currentTime = new_time;

        // FPS counter
        static double fps_timer = 0.0; static int fps_frames = 0;
        fps_timer += frame_time; fps_frames++;
        if (fps_timer >= 1.0) {
            std::string title = "Vectra Engine";
            glfwSetWindowTitle(window, title.c_str());
            fps_timer -= 1.0; fps_frames = 0;
        }

        accumulator += frame_time;
        while (accumulator >= dt_phys) {
            if (!state_.is_paused) scene->step(dt_phys * state_.simulation_speed);
            accumulator -= dt_phys;
            t += dt_phys;
        }

        // Clear the main window
        Renderer::begin_frame();

        // Create a scene snapshot
        auto scene_snapshot = scene->create_snapshot();

        // Render scene to framebuffer
        renderer->render_to_framebuffer(scene_snapshot, static_cast<linkit::real>(frame_time));

        // Draw UI with framebuffer texture
        ui->draw(state_, scene_snapshot, renderer->get_scene_texture_id());
        ui->end_frame();

        renderer->end_frame();

        double end_frame_time = glfwGetTime();
        double time_to_wait = min_frame_time - (end_frame_time - new_time);
        if (time_to_wait > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(time_to_wait));
        }
    }
    ui->cleanup();
    renderer->cleanup(*scene);
}

void Engine::physics_thread_func()
{
    using Clock = std::chrono::high_resolution_clock;
    using Duration = std::chrono::duration<double>;

    // Fixed physics time step (e.g., 60 Hz)
    const double dt = 1.0 / state_.simulation_frequency;

    auto current_time = Clock::now();
    double accumulator = 0.0;

    while (state_.is_running)
    {
        // Skip physics while scene is being restarted (handled by rendering thread)
        if (state_.scene_should_restart)
        {
            current_time = Clock::now();
            accumulator = 0.0;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        auto new_time = Clock::now();
        Duration frame_time = new_time - current_time;
        current_time = new_time;

        accumulator += frame_time.count();

        // Clamp accumulator to prevent "Spiral of Death" if physics falls behind
        if (accumulator > 0.25)
            accumulator = 0.25;

        // Only step physics when enough real time has accumulated
        while (accumulator >= dt)
        {
            if (!state_.is_paused) {
                scene->step(dt * state_.simulation_speed);
            }
            accumulator -= dt;
        }

        // Push state to renderer
        // Note: In a triple-buffer setup, this might block if the renderer is slow.
        // That is acceptable; the accumulator ensures we catch up when we unblock.
        render_queue_.push(scene->create_snapshot());
    }
}

void Engine::rendering_thread_func()
{

    linkit::real currentTime = glfwGetTime();

    const linkit::real min_frame_time = 1.0 / state_.target_fps;

    renderer->use_skybox();

    auto window = renderer->get_window();

    while (!glfwWindowShouldClose(window)) {
        // Handle scene restart on the rendering thread (has OpenGL context)
        if (state_.scene_should_restart)
        {
            if (!state_.requested_scene_file.empty()) {
                load_scene(state_.requested_scene_file);
                state_.requested_scene_file = "";
            } else
            {
                load_scene(state_.loaded_scene);
                state_.scene_should_restart = false;
            }
        }

        double new_time = glfwGetTime();
        double frame_time = new_time - currentTime;
        currentTime = new_time;


        SceneSnapshot scene_snapshot;
        render_queue_.pop(scene_snapshot);

        // Clear the main window
        Renderer::begin_frame();

        // Render scene to framebuffer
        renderer->render_to_framebuffer(scene_snapshot, static_cast<linkit::real>(frame_time));

        // Draw UI with framebuffer texture
        ui->draw(state_, scene_snapshot, renderer->get_scene_texture_id());
        ui->end_frame();

        renderer->end_frame();

        double end_frame_time = glfwGetTime();
        double time_to_wait = min_frame_time - (end_frame_time - new_time);
        if (time_to_wait > 0.0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(time_to_wait));
        }
    }
    // I'm aware of the possible race condition
    state_.is_running = false;
    ui->cleanup();
    renderer->cleanup(*scene);
}

void Engine::run()
{
    std::thread physics_thread(&Engine::physics_thread_func, this);

    // Rendering must run on the main thread due to GLFW constraints
    // (Event loop must run on the thread that created the window)
    rendering_thread_func();

    // Signal the queue to unblock any waiting threads so we can exit cleanly
    render_queue_.shutdown();

    if (physics_thread.joinable()) {
        physics_thread.join();
    }
}
