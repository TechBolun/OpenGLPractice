#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 uv;


uniform mat4 ShadowModelToWorldMatrix ;
uniform mat4 ShadowModelToProjectionMatrix ;

out vec3 normalWorld;
out vec3 worldPos;

void main()
{
	gl_Position = ShadowModelToProjectionMatrix * vec4(position, 1.0);
	normalWorld = vec3(ShadowModelToWorldMatrix * vec4(normalModel, 0));
	worldPos = vec3(ShadowModelToWorldMatrix * vec4(position, 1));
}