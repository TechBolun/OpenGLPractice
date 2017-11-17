#version 430

out vec4 daColor;

in mat4 tangentToModelMatrix;
in vec3 vertexPositionWorld;
in vec2 uv;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec4 ambientLight;

uniform sampler2D myTexture;
uniform sampler2D meNormal;

uniform mat4 modelToWorldMatrix;


void main()
{
	//Normal

	// get normal color from tangent space normal map
	vec4 normalColor = texture(meNormal, uv);

	//function 2v - 1   in order to get between -1 and 1
	vec3 normalTangentSpace = 2 * (normalColor.xyz - vec3(0.5f, 0.5f, 0.5f));

	vec3 normalModelSpace = vec3(tangentToModelMatrix * vec4(normalTangentSpace, 0.0));
	vec3 normalWorldSpace = vec3(modelToWorldMatrix * vec4(normalModelSpace, 1.0));



	// Diffuse
	vec3 lightVectorWorld = normalize(lightPosition - vertexPositionWorld);
	float brightness = clamp(dot(lightVectorWorld, normalize(normalWorldSpace)), 0, 1);
	//vec4 texColor = texture(myTexture, uv);
	vec4 texColor = vec4(1, 1, 1, 1);
	vec4 diffuseLight = texColor * brightness;

	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorldSpace);
	vec3 eyeVectorWorld = normalize(cameraPosition - vertexPositionWorld);
	float gloss = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	//gloss = pow(gloss, 50);
	vec4 specularLight = vec4(gloss, gloss, gloss, 1);

	//attenuation
	float atten = 0.01;
	float attenDistance = length(lightPosition - vertexPositionWorld);
	float powAtten = pow(attenDistance, 2);
	atten = 1.0 / (1.0 + atten * powAtten);

	//daColor = ambientLight + atten * (diffuseLight + specularLight);
	daColor = vec4(diffuseLight);
	//daColor  = vec4(uv,0,0);
}