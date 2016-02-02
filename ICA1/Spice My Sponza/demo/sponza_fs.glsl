#version 330

uniform float MAX_LIGHTS;

struct Light
{
    vec3 position;
    float range;
	vec3 intensity;
};

uniform Light LightSource[22];

uniform vec3 camera_position;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;

in vec3 vertexPos;
in vec3 normal;
in vec2 text_coord;

uniform vec3 vertex_diffuse_colour;
uniform vec3 vertex_spec_colour;
uniform vec3 vertex_ambient_colour;
uniform float vertex_shininess;
uniform float is_vertex_shiney;

out vec4 fragment_colour;

void main(void)
{	

	vec3 col = vec3(0, 0, 0);
	float diffuse_intensity = 0.f;

	vec3 N = normal;
	vec3 vertexToEye = normalize(camera_position - vertexPos);
	vec3 specularColour = vec3(0,0,0);
	
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		float att = 0.0f;
		vec3 LightPosition = LightSource[i].position;
		vec3 L = normalize(LightSource[i].position - vertexPos);
		float dist = distance(LightSource[i].position, vertexPos);
		att = 1-smoothstep(0.0, LightSource[i].range, dist);
		diffuse_intensity = max(0, dot(L, N)) * att;
		
		if(is_vertex_shiney > 0 && diffuse_intensity > 0)
		{
			vec3 lightReflection = normalize(reflect(L, normal));
			float specularFactor = dot(vertexToEye, lightReflection);
		
			if (specularFactor > 0)
			{
				specularFactor = pow(specularFactor, vertex_shininess)* att;
				specularColour = LightSource[i].intensity *  vertex_spec_colour * specularFactor  /** texture2D(specular_texture, text_coord).rgb*/;
				col += LightSource[i].intensity * diffuse_intensity * vertex_ambient_colour * specularColour;
			}
			else
			{
				col += LightSource[i].intensity * diffuse_intensity * vertex_ambient_colour ;
			}
		}
		else
		{
			col += LightSource[i].intensity * diffuse_intensity * vertex_ambient_colour;
		}
	}

	

	fragment_colour = vec4(col * vertex_diffuse_colour /** texture2D(diffuse_texture, text_coord).rgb*/, 1.0);
}
