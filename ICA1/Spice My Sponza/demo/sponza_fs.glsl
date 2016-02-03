#version 330

struct Light
{
    vec3 position;
    float range;
	vec3 intensity;
};

uniform float MAX_LIGHTS;
uniform Light LightSource[22];
uniform vec3 camera_position;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform float outline;

uniform vec3 vertex_diffuse_colour;
uniform vec3 vertex_spec_colour;
uniform vec3 vertex_ambient_colour;
uniform vec3 global_ambient_light;

uniform float vertex_shininess;
uniform float is_vertex_shiney;
uniform float has_diff_tex;

in vec3 vertexPos;
in vec3 vertexNormal;
in vec2 text_coord;

out vec4 fragment_colour;

void main(void)
{	
	//Wireframe rendering colour
	if (outline == 1)
	{
		fragment_colour = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		vec3 col = vec3(0, 0, 0);
		float diffuse_intensity = 0.f;

		vec3 N = normalize(vertexNormal);
		vec3 vertexToEye = normalize(camera_position - vertexPos);
		vec3 specularColour = vec3(0, 0, 0);

		for (int i = 0; i < MAX_LIGHTS; i++)
		{
			float att = 0.0f;
			vec3 LightPosition = LightSource[i].position;
			vec3 L = normalize(LightSource[i].position - vertexPos);
			float dist = distance(LightSource[i].position, vertexPos);
			att = 1 - smoothstep(0.0, LightSource[i].range, dist);

			if (att > 0)
			{

				diffuse_intensity = max(0, dot(L, N)) * att;

				if (is_vertex_shiney > 0 && diffuse_intensity > 0)
				{
					vec3 lightReflection = normalize(reflect(L, N));
					float specularFactor = max(0.0, dot(vertexToEye, lightReflection));

					if (specularFactor > 0)
					{
						specularFactor = pow(specularFactor, vertex_shininess);
						specularColour = LightSource[i].intensity *  vertex_spec_colour * specularFactor * att;
						col += LightSource[i].intensity * diffuse_intensity * specularColour * texture2D(specular_texture, text_coord).rgb;
						continue;
					}
				}
				col += LightSource[i].intensity * diffuse_intensity;
			}
		}
		col *= vertex_diffuse_colour + vertex_ambient_colour * global_ambient_light;

		if(has_diff_tex == 1)
			fragment_colour = vec4(col   * texture2D(diffuse_texture, text_coord).rgb, 1.0);
		else
			fragment_colour = vec4(col, 1.0);
	}
}
