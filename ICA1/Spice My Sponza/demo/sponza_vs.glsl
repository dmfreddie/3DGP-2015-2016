#version 330

uniform mat4 projection_view_model_xform;
uniform mat4 model_xform;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 vertex_tangent;
in vec2 vertex_texcoord;

out vec3 vertexPos;
out vec3 normal;
out vec2 text_coord;
out vec3 vertexNormal;

void main(void)
{	
	vertexNormal = mat3(model_xform) *  vertex_normal;
	vertexPos = mat4x3(model_xform) * vec4(vertex_position, 1.0);
    //vec3 tex_colour = texture(mystery_sampler, vertex_texcoord).rgb;
	normal = normalize(mat3(model_xform) *  vertex_normal);
	//text_colour = tex_colour;

	text_coord = vertex_texcoord;


	gl_Position = projection_view_model_xform * vec4(vertex_position, 1.0);
}
