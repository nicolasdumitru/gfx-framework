#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform int is_spot;
uniform float cut_off;

uniform vec3 object_color;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 N = normalize(world_normal);
    vec3 L = normalize(light_position - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    float ambient_light = 0.25;
    float diffuse_light = material_kd * max(dot(N, L), 0);
    float specular_light = 0;
    // It's important to distinguish between "reflection model" and
    // "shading method". In this shader, we are experimenting with the Phong
    // (1975) and Blinn-Phong (1977) reflection models, and we are using the
    // Phong (1975) shading method. Don't mix them up!
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
    }

    float spot = 1;
    if (is_spot == 1)
    {
        float spot_light = dot(-L, normalize(light_direction));
        float spot_light_limit = cos(cut_off);
        spot = 0;
        if (spot_light > spot_light_limit)
        {
            float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
            spot = pow(linear_att, 2);
        }
    }

    float d = distance(light_position, world_position);

    float att = 1 / (1 + .22 * d + .2 * d * d);
    float light = ambient_light + att * spot * (diffuse_light + specular_light);

    out_color = vec4(object_color * light, 1);
}
