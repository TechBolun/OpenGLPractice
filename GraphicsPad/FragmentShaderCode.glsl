#version 430

out vec4 daColor;
in vec3 normalWorld;
in vec3 vertexPositionWorld;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform vec4 ambientLight;

void main()
{
	// Diffuse
	vec3 lightVectorWorld = normalize(lightPosition - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld));
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);

	// Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
	vec3 eyeVectorWorld = normalize(cameraPosition - vertexPositionWorld);
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s, 50);
	vec4 specularLight = vec4(s, 0, 0, 1);

	//attenuation
	float atten = 0.2;
	float attenDistance = length(lightPosition - vertexPositionWorld);
	float powAtten = pow(attenDistance, 2);
	atten = 1.0 / (1.0 + atten * powAtten);

	daColor = ambientLight + atten * (clamp(diffuseLight, 0, 1) + specularLight);
}