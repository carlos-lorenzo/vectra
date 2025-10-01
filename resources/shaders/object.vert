#version 330 core
layout (location = 0) in vec3 aPos; // Local vertex position
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

// Uniforms for your transformation matrices
uniform mat4 model;      // Local -> World
uniform mat4 view;       // World -> View
uniform mat4 projection; // View  -> Clip

void main()
{
    // The final output must be in Clip Space.
    // OpenGL handles the division by 'w' automatically after this shader.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}