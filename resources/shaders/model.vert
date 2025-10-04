#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

// Uniforms for your transformation matrices
uniform mat4 model;      // Local -> World
uniform mat4 view;       // World -> View
uniform mat4 projection; // View  -> Clip

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}