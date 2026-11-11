#version 330

// Input
// Values interpolated across the triangle from the vertex shader outputs
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture_coord;
in vec3 frag_color;

uniform float Time;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // Use the vertex normal as the color.
    // Replace with `frag_color` to use the per-vertex color instead.
    vec3 color = frag_normal;

    // Bonus: pulse the red channel over time
    color.r = color.r * (0.5 + 0.5 * sin(Time * 3.0));

    out_color = vec4(color, 1);
}
