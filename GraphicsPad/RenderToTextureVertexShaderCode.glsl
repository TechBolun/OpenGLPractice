#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 texCoord;
in layout(location=4) vec4 tangent;

uniform mat4 renderTextureModelToProjectionMatrix;

out vec2 uv;

void main()
{
	gl_Position = renderTextureModelToProjectionMatrix * vec4(position, 1.0);
	uv = texCoord;
}