#version 330

uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 vertexNormal;

void main(void)
{	
	vertexNormal = mat3(model_xform) *  vertex_normal;

	gl_Position = vec4(vertex_position, 1.0);
}
