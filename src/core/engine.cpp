#include <thread>
#include <chrono>



#include "linkit/linkit.h"
#include "vectra/core/engine.h"

#include <iostream>
#include <ostream>

#include "vectra/core/render_queue.h"


#include "vectra/physics/forces/simple_gravity.h"



Engine::Engine() : render_queue_()
{
    state_ = EngineState();
    renderer = std::make_unique<Renderer>(&state_);
    scene = std::make_unique<Scene>();
    ui = std::make_unique<EngineUI>();
    EngineUI::initialize(renderer->get_window());

}

void Engine::load_scene()
{


    LightSource scene_light = LightSource(glm::vec3(0, 10, -10), glm::vec3(1.0, 1.0, 0.8));
    scene->add_light_source(scene_light);

    // Box dimensions
    float box_size = 20.0f;
    float wall_thickness = 1.0f;
    float wall_height = 20.0f;

    // Floor
    GameObject floor;
    floor.rb.mass = 0;
    floor.rb.inverse_mass = 0;
    floor.rb.transform.position = linkit::Vector3(0, -wall_height/2, 0);
    floor.rb.transform.scale = linkit::Vector3(box_size, wall_thickness, box_size);
    floor.set_collider_type("ColliderBox");
    floor.set_shape("cube");
    scene->add_game_object(floor);

    // Front wall (positive Z)
    GameObject front_wall;
    front_wall.rb.mass = 0;
    front_wall.rb.inverse_mass = 0;
    front_wall.rb.transform.position = linkit::Vector3(0, 0, box_size/2);
    front_wall.rb.transform.scale = linkit::Vector3(box_size, wall_height, wall_thickness);
    front_wall.set_collider_type("ColliderBox");
    front_wall.set_shape("cube");
    scene->add_game_object(front_wall);

    // Back wall (negative Z)
    GameObject back_wall;
    back_wall.rb.mass = 0;
    back_wall.rb.inverse_mass = 0;
    back_wall.rb.transform.position = linkit::Vector3(0, 0, -box_size/2);
    back_wall.rb.transform.scale = linkit::Vector3(box_size, wall_height, wall_thickness);
    back_wall.set_collider_type("ColliderBox");
    back_wall.set_shape("cube");
    scene->add_game_object(back_wall);

    // Left wall (negative X)
    GameObject left_wall;
    left_wall.rb.mass = 0;
    left_wall.rb.inverse_mass = 0;
    left_wall.rb.transform.position = linkit::Vector3(-box_size/2, 0, 0);
    left_wall.rb.transform.scale = linkit::Vector3(wall_thickness, wall_height, box_size);
    left_wall.set_collider_type("ColliderBox");
    left_wall.set_shape("cube");
    scene->add_game_object(left_wall);

    // Right wall (positive X)
    GameObject right_wall;
    right_wall.rb.mass = 0;
    right_wall.rb.inverse_mass = 0;
    right_wall.rb.transform.position = linkit::Vector3(box_size/2, 0, 0);
    right_wall.rb.transform.scale = linkit::Vector3(wall_thickness, wall_height, box_size);
    right_wall.set_collider_type("ColliderBox");
    right_wall.set_shape("cube");
    scene->add_game_object(right_wall);

    GameObject ceiling;
    floor.rb.mass = 0;
    floor.rb.inverse_mass = 0;
    floor.rb.transform.position = linkit::Vector3(0, wall_height/2, 0);
    floor.rb.transform.scale = linkit::Vector3(box_size, wall_thickness, box_size);
    floor.set_collider_type("ColliderBox");
    floor.set_shape("cube");
    scene->add_game_object(floor);


    // Add spheres inside the box
    int n_spheres = 30;
    linkit::Vector3 gravity = linkit::Vector3(0, -9.8, 0.0);

    for (int i = 0; i < n_spheres; i++)
    {
        GameObject sphere;
        sphere.set_collider_type("ColliderSphere");
        sphere.set_shape("sphere");
        // Position spheres at different heights inside the box
        float x_pos = (i % 3 - 1) * 1.0f;
        float z_pos = ((i / 3) % 3 - 1) * 1.0f;
        float y_pos = -10.0f + (i * 0.5f);
        sphere.rb.transform.position = linkit::Vector3(x_pos, y_pos, z_pos);
        // Give spheres some initial velocity for movement
        sphere.rb.velocity = 5* linkit::Vector3((i % 2 == 0 ? 2.0f : -2.0f), 0, (i % 3 == 0 ? 1.5f : -1.5f));
        scene->add_game_object(sphere);

        // Apply gravity to each sphere (index offset by 6 for the 6 box pieces: floor + 4 walls + ceiling)
        scene->force_registry.add(&scene->game_objects[6 + i], std::make_shared<SimpleGravity>(gravity));
    }

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

        // Create scene snapshot
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
