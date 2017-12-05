#version 430

in vec3 theColor;
in mat4 tangentToModelMatrix;
in vec3 worldPos;
in vec2 uv;
in vec3 FragNormal;

uniform vec3 lightPosition;
uniform vec3 eyePosition;
uniform vec4 ambientLight;

uniform sampler2D myTexture;
uniform sampler2D meNormal;
uniform samplerCube meCubeMap;

uniform mat4 reflectionModelToWorldMatrix;

out vec4 daColor;

void main()
{
	vec3 normalWorldSpace = vec3(reflectionModelToWorldMatrix * vec4(FragNormal, 1.0));

	vec3 viewDir = normalize(eyePosition - worldPos);

	//CubeMap Reflection
	vec3 reflectDir = reflect(-viewDir, normalize(normalWorldSpace));
	vec4 cubeMapColor = texture(meCubeMap, reflectDir);

	//daColor = vec4(viewDir, 1.0);
	daColor = cubeMapColor;
}