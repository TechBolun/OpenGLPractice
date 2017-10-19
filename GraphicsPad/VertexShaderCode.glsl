#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;

uniform mat4 transform;

out vec3 theColor;

void main()
{
	gl_Position = transform * vec4(position, 1.0);
	theColor = vertexColor;
}