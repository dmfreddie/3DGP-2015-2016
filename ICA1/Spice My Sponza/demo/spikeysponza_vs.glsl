#version 330

uniform mat4 projection_view_model_xform;
uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 vertexPos;
out vec3 vertexNormal;

void main(void)
{	
	vertexNormal = mat3(model_xform) *  vertex_normal;
	vertexPos = mat4x3(model_xform) * vec4(vertex_position, 1.0);

	gl_Position = vec4(vertex_position, 1.0);
}
