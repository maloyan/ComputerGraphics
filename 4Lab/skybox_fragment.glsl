#version 330 core

in vec3 vTexCoords;

out vec4 color;

uniform samplerCube textureSky;

void main()
{
	
	color = texture(textureSky, vTexCoords);
}