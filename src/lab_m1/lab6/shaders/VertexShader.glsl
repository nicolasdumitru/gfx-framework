#version 330

// Input
// Vertex attributes, one per pipe, matching the glVertexAttribPointer
// calls in Lab6::CreateMesh (and the layout used by Mesh::LoadMesh)
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture_coord;
out vec3 frag_color;

void main()
{
    // Send output to fragment shader
    frag_position = v_position;
    frag_normal = v_normal;
    frag_texture_coord = v_texture_coord;
    frag_color = v_color;

    // Bonus: make the object bob up and down over time
    vec3 position = v_position + vec3(0, 0.25 * sin(Time * 2.0), 0);

    // Compute gl_Position
    gl_Position = Projection * View * Model * vec4(position, 1.0);
}
