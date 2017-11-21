#version 430

in vec3 texcoord;
in vec3 color;

uniform samplerCube meCubeMap;

out vec4 daColor;

void main()
{
	daColor = texture(meCubeMap, texcoord);
	//daColor = vec4(color,1.0);
}