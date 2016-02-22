#version 330

uniform mat4 projection_view_world_xform;

in vec3 vertex_position;

void main(void)
{
    gl_Position = projection_view_world_xform * vec4(vertex_position, 1.0);
}
