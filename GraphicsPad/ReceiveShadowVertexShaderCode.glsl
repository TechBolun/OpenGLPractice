#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 texCoord;


uniform mat4 ShadowModelToWorldMatrix ;
uniform mat4 ShadowModelToProjectionMatrix ;

out vec3 Normal;
out vec3 worldPos;
out vec2 uv;

void main()
{
	gl_Position = ShadowModelToProjectionMatrix * vec4(position, 1.0);
	Normal = vec3(ShadowModelToWorldMatrix * vec4(normalModel, 0));
	worldPos = vec3(ShadowModelToWorldMatrix * vec4(position, 1));
	uv = texCoord;
}