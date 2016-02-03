#version 330

layout (points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 projection_view_model_xform;
uniform mat4 normal_xform;
uniform float normal_vector_length;

in vec3 vertexNormal[];
out vec3 normal;

void main(void)
{	
	//Get the normal vector from the first element in the normal array and same for position
	normal = vertexNormal[0];
	vec3 vertexPos = gl_in[0].gl_Position.xyz;

	//Calculate the end point of the line along the vertex normal
	vec3 vertexNormal = (normal_xform * vec4(vertexNormal[0] * normal_vector_length, 1.0)).xyz;

	//Set the first point and emit the first vertex
	gl_Position =  projection_view_model_xform * vec4(vertexPos, 1.0);
	EmitVertex();

	//Set the second point and emit the last / second vertex
	gl_Position = projection_view_model_xform * vec4(vertexPos + vertexNormal, 1.0);
	EmitVertex();

	//End the primitive to draw th line in the fragment shader
	EndPrimitive();
}
