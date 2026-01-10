#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;   // bound to texture unit 0

void main()
{

    // Will be passed by uniform
    vec3 light_colour = vec3(1.0, 1.0, 1.0);
    vec3 light_position = vec3(0, 50, 10);
    float ambient_strenght = 0.5;

    // Albedo from texture (0..1)
    vec4 albedo = texture(texture_diffuse1, TexCoords);

    // Ambient
    vec3 ambient = ambient_strenght * light_colour;

    // Diffuse (Lambert)
    vec3 N = normalize(Normal);
    vec3 L = normalize(light_position - FragPos);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * light_colour;

    // Combine and keep texture alpha
    vec3 color = (ambient + diffuse) * albedo.rgb;
    FragColor = vec4(color, albedo.a);
}