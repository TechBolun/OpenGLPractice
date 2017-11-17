#version 430

in vec3 texcoord;

uniform samplerCube meCubeMap;

out vec4 daColor;

void main()
{
	daColor = texture(meCubeMap, texcoord);
}