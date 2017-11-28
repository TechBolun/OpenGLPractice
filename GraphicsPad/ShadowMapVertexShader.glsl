#version 430

in layout(location=0) vec3 vertexPosition;
in layout(location=3) vec2 uv;

uniform mat4 modelToProjectionMatrix;

out vec2 texCoord;

void main() {
	
	gl_Position = modelToProjectionMatrix * vec4(vertexPosition, 1.0);
	texCoord = uv;
}

