#include "gameobject.h"
#include "vectra/rendering/renderer.h"
#include "vectra/rendering/light_source.h"
#include "vectra/physics/forces/simple_gravity.h"
#include "linkit/linkit.h"



int main()
{
    Renderer scene_renderer(1920, 1080);
    Scene scene;

    LightSource scene_light = LightSource(glm::vec3(0, 10, -10), glm::vec3(1.0, 1.0, 0.8));
    scene.add_light_source(scene_light);
    scene.camera.transform.position = linkit::Vector3(0.0f, 5.0f, 20.0f);

    // Define gravity
    linkit::Vector3 gravity_vec(0, -9.8, 0.0);
    auto gravity = std::make_shared<SimpleGravity>(gravity_vec);

    // Floor
    GameObject floor;
    floor.rb.mass = 0;
    floor.rb.inverse_mass = 0;
    floor.rb.transform.position = linkit::Vector3(0, 0, 0); // Thickness 1.0, so top at 0
    floor.rb.transform.scale = linkit::Vector3(50.0f, 1.0f, 50.0f);
    floor.set_collider_type("ColliderBox");
    floor.set_shape("cube");
    scene.add_game_object(floor);

    // Tall block tower
    GameObject tower;
    tower.rb.mass = 50.0f;
    tower.rb.inverse_mass = 1.0f / tower.rb.mass;
    tower.rb.transform.scale = linkit::Vector3(3.0f, 2.0f, 2.0f);
    tower.rb.transform.position = linkit::Vector3(0.0f, 5.0f, 0.0f); // y = height/2

    tower.set_collider_type("ColliderBox");
    tower.set_shape("cube");
    scene.add_game_object(tower);

    // Ball launched at tower
    GameObject ball;
    ball.rb.mass = 2.0f;
    ball.rb.inverse_mass = 1.0f / ball.rb.mass;


    // Position
    float ball_z = 15.0f;
    float ball_y = 5.0f; // Aim at middle height of tower
    ball.rb.transform.position = linkit::Vector3(0.5f, ball_y, ball_z);
    ball.rb.transform.scale = linkit::Vector3(1.0f, 1.0f, 1.0f);

    // Velocity
    ball.rb.velocity = linkit::Vector3(0, 0, -35.0f);

    ball.set_collider_type("ColliderSphere");
    ball.set_shape("sphere");
    scene.add_game_object(ball);

    // Apply gravity to all dynamic objects (SKIP FLOOR at index 0)
    for (size_t i = 1; i < scene.game_objects.size(); ++i)
    {
        scene.force_registry.add(&scene.game_objects[i], gravity);
    }

    scene_renderer.play_scene(scene);
    return 0;
}
