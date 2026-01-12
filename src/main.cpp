#include "gameobject.h"
#include "vectra/rendering/renderer.h"
#include "vectra/rendering/light_source.h"
#include "vectra/physics/forces/simple_gravity.h"
#include "vectra/physics/forces/newtonian_gravity.h"
#include "vectra/physics/forces/anchored_spring.h"
#include "linkit/linkit.h"
#include "vectra/core/engine.h"


int main()
{
    // Renderer scene_renderer(1920, 1080);
    // Scene scene;
    //
    // LightSource scene_light = LightSource(glm::vec3(0, 10, -10), glm::vec3(1.0, 1.0, 0.8));
    // scene.add_light_source(scene_light);
    //
    //
    //
    //
    // // Box dimensions
    // float box_size = 20.0f;
    // float wall_thickness = 1.0f;
    // float wall_height = 20.0f;
    //
    // // Floor
    // GameObject floor;
    // floor.rb.mass = 0;
    // floor.rb.inverse_mass = 0;
    // floor.rb.transform.position = linkit::Vector3(0, -wall_height/2, 0);
    // floor.rb.transform.scale = linkit::Vector3(box_size, wall_thickness, box_size);
    // floor.set_collider_type("ColliderBox");
    // floor.set_shape("cube");
    // scene.add_game_object(floor);
    //
    // // Front wall (positive Z)
    // GameObject front_wall;
    // front_wall.rb.mass = 0;
    // front_wall.rb.inverse_mass = 0;
    // front_wall.rb.transform.position = linkit::Vector3(0, 0, box_size/2);
    // front_wall.rb.transform.scale = linkit::Vector3(box_size, wall_height, wall_thickness);
    // front_wall.set_collider_type("ColliderBox");
    // front_wall.set_shape("cube");
    // scene.add_game_object(front_wall);
    //
    // // Back wall (negative Z)
    // GameObject back_wall;
    // back_wall.rb.mass = 0;
    // back_wall.rb.inverse_mass = 0;
    // back_wall.rb.transform.position = linkit::Vector3(0, 0, -box_size/2);
    // back_wall.rb.transform.scale = linkit::Vector3(box_size, wall_height, wall_thickness);
    // back_wall.set_collider_type("ColliderBox");
    // back_wall.set_shape("cube");
    // scene.add_game_object(back_wall);
    //
    // // Left wall (negative X)
    // GameObject left_wall;
    // left_wall.rb.mass = 0;
    // left_wall.rb.inverse_mass = 0;
    // left_wall.rb.transform.position = linkit::Vector3(-box_size/2, 0, 0);
    // left_wall.rb.transform.scale = linkit::Vector3(wall_thickness, wall_height, box_size);
    // left_wall.set_collider_type("ColliderBox");
    // left_wall.set_shape("cube");
    // scene.add_game_object(left_wall);
    //
    // // Right wall (positive X)
    // GameObject right_wall;
    // right_wall.rb.mass = 0;
    // right_wall.rb.inverse_mass = 0;
    // right_wall.rb.transform.position = linkit::Vector3(box_size/2, 0, 0);
    // right_wall.rb.transform.scale = linkit::Vector3(wall_thickness, wall_height, box_size);
    // right_wall.set_collider_type("ColliderBox");
    // right_wall.set_shape("cube");
    // scene.add_game_object(right_wall);
    //
    // GameObject ceiling;
    // floor.rb.mass = 0;
    // floor.rb.inverse_mass = 0;
    // floor.rb.transform.position = linkit::Vector3(0, wall_height/2, 0);
    // floor.rb.transform.scale = linkit::Vector3(box_size, wall_thickness, box_size);
    // floor.set_collider_type("ColliderBox");
    // floor.set_shape("cube");
    // scene.add_game_object(floor);
    //
    //
    // // Add spheres inside the box
    // int n_spheres = 10;
    // linkit::Vector3 gravity = linkit::Vector3(0, -9.8, 0.0);
    //
    // for (int i = 0; i < n_spheres; i++)
    // {
    //     GameObject sphere;
    //     sphere.set_collider_type("ColliderSphere");
    //     sphere.set_shape("sphere");
    //     // Position spheres at different heights inside the box
    //     float x_pos = (i % 3 - 1) * 3.0f;
    //     float z_pos = ((i / 3) % 3 - 1) * 3.0f;
    //     float y_pos = -10.0f + (i * 2.0f);
    //     sphere.rb.transform.position = linkit::Vector3(x_pos, y_pos, z_pos);
    //     // Give spheres some initial velocity for movement
    //     sphere.rb.velocity = 5* linkit::Vector3((i % 2 == 0 ? 2.0f : -2.0f), 0, (i % 3 == 0 ? 1.5f : -1.5f));
    //     scene.add_game_object(sphere);
    //
    //     // Apply gravity to each sphere (index offset by 6 for the 6 box pieces: floor + 4 walls + ceiling)
    //     scene.force_registry.add(&scene.game_objects[6 + i], std::make_shared<SimpleGravity>(gravity));
    // }
    //
    // scene_renderer.play_scene(scene);
    Engine* engine = new Engine();
    engine->load_scene();
    engine->run();
    delete engine;
    return 0;
}

