#version 430

in vec3 worldPos;
in vec2 uv;
in mat4 tangentToModelMatrix;
in vec3 Normal;

uniform sampler2D renderedTexture;

out vec4 daColor;

float luminance(vec3 color) {
	
	return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}
void main() {
	
	float dx = 1.0 / 160;
	float dy = 1.0 / 100;

	float s00 = luminance(texture(renderedTexture, uv + vec2(-dx, dy)).rgb);
	float s10 = luminance(texture(renderedTexture, uv + vec2(-dx, 0.0)).rgb);
	float s20 = luminance(texture(renderedTexture, uv + vec2(-dx, -dy)).rgb);
	float s01 = luminance(texture(renderedTexture, uv + vec2(0.0, dy)).rgb);
	float s21 = luminance(texture(renderedTexture, uv + vec2(0.0, -dy)).rgb);
	float s02 = luminance(texture(renderedTexture, uv + vec2(dx, dy)).rgb);

	float s12 = luminance(texture(renderedTexture, uv + vec2(dx, 0.0)).rgb);
	float s22 = luminance(texture(renderedTexture, uv + vec2(dx, -dy) ).rgb);

	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);

	float dist = sx * sx + sy * sy;
	if( dist > 0.3f )
		daColor = vec4(0.0,0.0,0.0,1.0);
	else
		daColor = vec4(1.0);
}