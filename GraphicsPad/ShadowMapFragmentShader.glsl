#version 430

in vec2 texCoord;

out daColor;

uniform sampler2D shadowMap;

void main() {
	daColor = vec4(shadowMap, 1.0);
}