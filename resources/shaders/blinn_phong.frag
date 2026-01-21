#version 330 core
out vec4 frag_colour;


struct DirectionalLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float strength;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cut_off;
    float outer_cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};




#define MAX_DIRECTIONAL_LIGHTS 10
#define MAX_POINT_LIGHTS 10
#define MAX_SPOT_LIGHTS 10

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform int num_directional_lights;
uniform int num_point_lights;
uniform int num_spot_lights;

uniform DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

uniform sampler2D texture_diffuse1;   // bound to texture unit 0
uniform vec3 view_pos;

float shininess = 32.0;

// Constants for slight PBR approximation
const float kPi = 3.14159265;
const float kShininess = 16.0;


vec3 calc_dir_light(DirectionalLight light, vec3 normal_f, vec3 view_dir_f);
vec3 calc_point_light(PointLight light, vec3 normal_f, vec3 frag_pos_f, vec3 view_dir_f);
vec3 calc_spot_light(SpotLight light, vec3 normal_f, vec3 frag_pos_f, vec3 view_dir_f);
float blinn_specular(vec3 light_dir, vec3 normal_f, vec3 view_dir_f);

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 view_dir = normalize(view_pos - FragPos);

    // phase 1: directional lighting
    vec3 colour = vec3(0.0);
    for(int i = 0; i < min(num_directional_lights, MAX_DIRECTIONAL_LIGHTS); i++)
    colour += calc_dir_light(directional_lights[i], norm, view_dir);

    // phase 2: point lights
    for(int i = 0; i < min(num_point_lights, MAX_POINT_LIGHTS); i++)
    colour += calc_point_light(point_lights[i], norm, FragPos, view_dir);

    // phase 3: spot light
    for(int i = 0; i < min(num_spot_lights, MAX_SPOT_LIGHTS); i++)
    colour += calc_spot_light(spot_lights[i], norm, FragPos, view_dir);

    // texture
    vec4 albedo = texture(texture_diffuse1, TexCoords);
    colour *= albedo.rgb;

    frag_colour = vec4(colour, albedo.a);
}

vec3 calc_dir_light(DirectionalLight light, vec3 normal_f, vec3 view_dir_f) {
    vec3 light_dir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal_f, light_dir), 0.0);

    // Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal_f);
    float spec = blinn_specular(light_dir, normal_f, view_dir_f);

    // Combine results
    vec3 ambient = light.ambient * light.strength;
    vec3 diffuse = light.diffuse * diff;
    vec3 specular = light.specular * spec * light.strength;
    return (ambient + diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal_f, vec3 frag_pos_f, vec3 view_dir_f) {
    vec3 light_dir = normalize(light.position - frag_pos_f);

    // Diffuse shading
    float diff = max(dot(normal_f, light_dir), 0.0);

    // Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal_f);
    float spec = blinn_specular(light_dir, normal_f, view_dir_f);

    // Attenuation
    float distance = length(light.position - frag_pos_f);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * attenuation;
    vec3 diffuse = light.diffuse * diff * attenuation;
    vec3 specular = light.specular * spec * attenuation;
    return (ambient + diffuse + specular);
}

vec3 calc_spot_light(SpotLight light, vec3 normal_f, vec3 frag_pos_f, vec3 view_dir_f) {
    vec3 light_dir = normalize(light.position - frag_pos_f);

    // Diffuse shading
    float diff = max(dot(normal_f, light_dir), 0.0);

    // Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal_f);
    float spec = blinn_specular(light_dir, normal_f, view_dir_f);

    // Attenuation
    float distance = length(light.position - frag_pos_f);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // Spotlight intensity
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = light.cut_off - light.outer_cut_off;
    float intensity = clamp((theta - light.outer_cut_off) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambient = light.ambient * attenuation * intensity;
    vec3 diffuse = light.diffuse * diff * attenuation * intensity;
    vec3 specular = light.specular * spec * attenuation * intensity;
    return (ambient + diffuse + specular);
}

float blinn_specular(vec3 light_dir, vec3 normal_f, vec3 view_dir_f) {
    const float k_energy_conservation = ( 8.0 + kShininess ) / ( 8.0 * kPi );
    vec3 halfway_dir = normalize(light_dir + view_dir_f);
    float spec = k_energy_conservation * pow(max(dot(normal_f, halfway_dir), 0.0), kShininess);
    return spec;
}