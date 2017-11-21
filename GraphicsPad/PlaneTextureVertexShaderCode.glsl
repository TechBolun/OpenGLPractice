#version 430

in layout(location=0) vec3 pos;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;

uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;

out vec3 vertexPositionWorld;
out vec2 uv;
out mat4 tangentToModelMatrix;


void main()
{
	gl_Position = modelToProjectionMatrix * vec4(pos, 1.0);
	vertexPositionWorld = vec3(modelToWorldMatrix * vec4(pos, 1.0));
}