#version 430

in layout(location=0) vec4 vertexPositionModel;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 VertextexCoord;


uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;


out vec3 normalWorld;
out vec3 vertexPositionWorld;
out vec2 textureCoordinate;

void main()
{
	gl_Position = modelToProjectionMatrix * vertexPositionModel;
	normalWorld = vec3(modelToWorldMatrix * vec4(normalModel, 0));
	vertexPositionWorld = vec3(modelToWorldMatrix * vertexPositionModel);

	textureCoordinate = VertextexCoord;
}