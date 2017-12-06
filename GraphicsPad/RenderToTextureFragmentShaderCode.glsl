#version 430

in vec2 uv;

out vec4 daColor;

uniform sampler2D myRenderedTexture;

void main()
{
	daColor = texture(myRenderedTexture, uv);
}