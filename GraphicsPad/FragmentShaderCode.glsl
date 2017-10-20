#version 430

out vec4 daColor;
in vec3 theNormal;
in vec3 thePosition;
in vec3 theColor;

uniform vec3 lightPosition;

void main()
{
	vec3 lightVector = normalize(lightPosition - thePosition);
	float brightness = dot(lightVector, theNormal);
	//daColor = vec4(theColor  * brightness, 1.0);
	//daColor = vec4(lightVector, 1.0);
	daColor = vec4(brightness, brightness, brightness, 1.0);
}