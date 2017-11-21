#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 uv;

uniform mat4 skyboxTransformMatrix;
out vec3 texcoord;
out vec3 color;

void main() {
	vec4 pos = vec4(position, 1.0);
	gl_Position = skyboxTransformMatrix * pos;
	texcoord = vec3(position.x, -position.yz);
	color = vertexColor;
}