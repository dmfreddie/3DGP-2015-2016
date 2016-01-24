#version 330

uniform float time_seconds;
uniform mat4 projection_view_model_xform;
uniform mat4 projection_xform;
uniform mat4 view_xform;
uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 vertex_tangent;
in vec2 vertex_texcoord;

out vec3 vertexPosition;
out vec3 vertexNormal;

void main(void)
{
	vertexPosition = vertex_position;
	vertexNormal = mat3(model_xform) * vertex_normal;
	
    gl_Position = projection_view_model_xform * vec4(vertex_position, 1.0);
}
