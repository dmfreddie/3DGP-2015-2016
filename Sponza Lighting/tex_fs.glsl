#version 330

struct Light
{
    vec3 position;
    float range;
    vec3 direction;
    float field_of_view;
    mat4 projection_view_xform;
};

uniform float time_seconds;
uniform vec3 camera_position;
uniform Light light_source;
uniform sampler2D sampler_tex0;
uniform sampler2D sampler_tex1;
uniform sampler2D sampler_tex2;

out vec4 fragment_colour;

in vec3 vertexPosition;
in vec3 vertexNormal;

void main(void)
{
	vec3 P = vertexPosition;
	vec3 N = normalize(vertexNormal);
	
	vec3 L = normalize(light_source.position – vertexPosition);
	
	float diffuse_intensity = max(0, dot(L, N));
	
	reflected_light = vec4(1.0,1.0,1.0,1.0) * diffuse_intensity;
	
    fragment_colour = vec4(1.0, 0.33, 0.0, 1.0) * reflected_light;
}
