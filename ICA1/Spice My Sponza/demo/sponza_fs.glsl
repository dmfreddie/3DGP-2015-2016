#version 330

uniform float MAX_LIGHTS;

struct Light
{
    vec3 position;
    float range;
    vec3 direction;
    float field_of_view;
    mat4 projection_view_xform;
	vec3 intensity;
} ;

uniform Light LightSource[22];

uniform vec3 camera_position;

uniform sampler2D mystery_sampler;
uniform sampler2D mystery_sampler1;
uniform sampler2D mystery_sampler2;
uniform sampler2D mystery_sampler3;

uniform vec3 positions[22];
uniform vec3 intensities[22];
uniform float ranges[22];

in vec3 vertexPos;
in vec3 normal;
in vec2 text_coord;

out vec4 fragment_colour;


void main(void)
{	
    vec3 col = vec3(0,0,0);
	float diffuse_intensity = 0.f;
	vec3 temp;

	vec3 N = normalize(normal);
	

	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		float att = 0.0f;
		vec3 temp;
		vec3 LightPosition = positions[i];
		vec3 L = normalize(positions[i] - vertexPos);
		float dist = distance(positions[i], vertexPos);
		att =  clamp(1.0 - dist*dist / (ranges[i] * ranges[i]), 0.0, 1.0); att *= att;
		diffuse_intensity =  max(0, dot(L, N)) * att;
		col += intensities[i] * diffuse_intensity;
	}
	col += 0.2f;
	fragment_colour =/* texture(mystery_sampler, text_coord)  * texture(mystery_sampler1, text_coord) **/ texture(mystery_sampler2, text_coord) *  texture(mystery_sampler3, text_coord)* vec4(col, 1.0);
}
