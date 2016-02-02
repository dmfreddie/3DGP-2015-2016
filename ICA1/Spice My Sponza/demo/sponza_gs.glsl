#version 330

layout (points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 projection_view_model_xform;
uniform mat4 model_xform;
uniform mat4 projection_xform;
uniform mat4 view_xform;
uniform mat4 normal_xform;

in vec3 vertexNormal[];
out vec3 normal;

uniform float normal_vector_length;

void main(void)
{	
	normal = vertexNormal[0];
	vec3 vertexPos = gl_in[0].gl_Position.xyz;
	vec3 vertexNormal = (normal_xform * vec4(vertexNormal[0] * 1, 1.0)).xyz;

	gl_Position =  projection_view_model_xform * vec4(vertexPos, 1.0);
	EmitVertex();

	gl_Position = projection_view_model_xform * vec4(vertexPos + vertexNormal, 1.0);
	EmitVertex();

	EndPrimitive();
}
