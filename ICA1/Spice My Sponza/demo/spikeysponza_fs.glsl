#version 330

out vec4 fragment_colour;

in vec3 normal;

void main(void)
{	
	//Set the colour of the normal vector line to the colour of the normal at that vertex position
	fragment_colour = vec4(normal * 0.5+0.5, 1.0);
}
