#version 430

in mat4 tangentToModelMatrix;
in vec3 vertexPositionWorld;
in vec2 uv;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec4 ambientLight;

uniform sampler2D myTexture;
uniform sampler2D meNormal;
uniform samplerCube meCubeMap;

uniform mat4 modelToWorldMatrix;

out vec4 daColor;


void main()
{
	//Normal

	// get normal color from tangent space normal map
	vec4 normalColor = texture(meNormal, uv);

	//function 2v - 1   in order to get between -1 and 1
	vec3 normalTangentSpace = 2 * (normalColor.xyz - vec3(0.5f, 0.5f, 0.5f));

	vec3 normalModelSpace = vec3(tangentToModelMatrix * vec4(normalTangentSpace, 0.0));
	vec3 normalWorldSpace = vec3(modelToWorldMatrix * vec4(normalModelSpace, 1.0));


	//attenuation
	float atten = 0.5;
	float attenDistance = length(lightPosition - vertexPositionWorld);
	float powAtten = pow(attenDistance, 2);
	atten = 1.0 / (1.0 + atten * powAtten);

	// Diffuse
	vec3 lightVectorWorld = normalize(lightPosition - vertexPositionWorld);
	float brightness = clamp(dot(vertexPositionWorld, normalize(normalWorldSpace)), 0, 1);
	vec4 texColor = texture(myTexture, uv);
	vec4 diffuseLight = texColor * brightness;

	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorldSpace);
	vec3 viewDir = normalize(cameraPosition - vertexPositionWorld);
	float gloss = clamp(dot(reflectedLightVectorWorld, viewDir), 0, 1);
	gloss = pow(gloss, 100);
	vec4 specularLight = vec4(gloss, gloss, gloss, 1);

	//CubeMap Reflection
	vec3 reflectDir = reflect(-viewDir, normalize(normalWorldSpace));
	//vec4 cubeMapColor = texture(meCubeMap, vec3(reflectDir.x, -reflectDir.yz));
	vec4 cubeMapColor = texture(meCubeMap, reflectDir);

	vec4 materialColor = ambientLight + atten * (diffuseLight + specularLight);
	//daColor = mix(materialColor, cubeMapColor, 0.5);
	daColor = materialColor;
	//daColor  = vec4(vertexPositionWorld,0);
	//daColor = ambientLight + atten * (diffuseLight);
	//daColor = ambientLight*texColor + (diffuseLight);
}