#version 430

in vec3 Normal;
in vec3 worldPos;
in vec2 uv;

uniform vec3 lightPosition;
uniform vec4 ambientLight;
uniform vec3 cameraPosition;

uniform sampler2D myShadowMap;
uniform sampler2D myTexture;

uniform mat4 LightModelToProjectionMatrix;
uniform mat4 ShadowModelToWorldMatrix;

out vec4 daColor;

void main()
{
	vec4 lightScreenPos = LightModelToProjectionMatrix * vec4(worldPos, 1);

	//attenuation
	float atten = 0.5;
	float attenDistance = length(lightPosition - worldPos);
	float powAtten = pow(attenDistance, 2);
	atten = 1.0 / (1.0 + atten * powAtten);

	//Shadow
	vec2 shadowMap_UV = vec2(lightScreenPos.x / lightScreenPos.w, lightScreenPos.y / lightScreenPos.w);
	vec2 shadowMap_UV_Fixed = shadowMap_UV * 0.5 + 0.5;
	vec4 shadowMap_Color = texture(myShadowMap, shadowMap_UV_Fixed);

	float shadow;

	//normalize shadow
	if(shadowMap_UV_Fixed.x >=1 || shadowMap_UV_Fixed.x <= 0 || 
		shadowMap_UV_Fixed.y >= 1 || shadowMap_UV_Fixed.y <= 0) {

		shadow = 1;
	} else {
		shadow = clamp((shadowMap_Color.x - lightScreenPos.z / lightScreenPos.w ) * 100 , 0, 1);
	}

	// Diffuse
	vec3 normalWorld = vec3(ShadowModelToWorldMatrix * vec4(Normal, 1.0));
	vec3 lightVectorWorld = normalize(lightPosition - worldPos);
	float brightness = clamp(dot(worldPos, normalize(normalWorld)), 0, 1);
	vec4 texColor = texture(myTexture, uv);


	vec4 diffuse = texColor * atten;



	//daColor = texture(myShadowMap, uv);
	daColor = diffuse * shadow;
	

}