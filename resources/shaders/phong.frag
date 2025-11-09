#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;   // bound to texture unit 0
uniform vec3 camera_position;
uniform vec3 light_colour;
uniform vec3 light_position;


void main()
{
    float ambient_strenght = 0.5;
    float specular_strength = 0.5;

    // Albedo from texture (0..1)
    vec4 albedo = texture(texture_diffuse1, TexCoords);

    // Ambient
    vec3 ambient = ambient_strenght * light_colour;

    // Diffuse (Lambert)
    vec3 normalised_normal = normalize(Normal);
    vec3 light_direction = normalize(light_position - FragPos);
    float diff = max(dot(normalised_normal, light_direction), 0.0);
    vec3 diffuse = diff * light_colour;


    // Specular
    vec3 viewDir = normalize(camera_position - FragPos);
    vec3 reflectDir = reflect(-light_direction, normalised_normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_colour;


    // Combine and keep texture alpha
    vec3 color = (ambient + diffuse + specular) * albedo.rgb;
    FragColor = vec4(color, albedo.a);
}