#version 430

in layout(location=0) vec3 pos;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 vertexTexCoord;
in layout(location=4) vec4 tangent;

uniform mat4 modelToProjectionMatrix;
uniform mat4 modelToWorldMatrix;

out vec3 vertexPositionWorld;
out vec2 uv;
out mat4 tangentToModelMatrix;

void main()
{
	gl_Position = modelToProjectionMatrix * vec4(pos, 1.0);
	vertexPositionWorld = vec3(modelToWorldMatrix * vec4(pos, 1.0));

	uv = vertexTexCoord;

	// return normalized vec3
	vec3 bitangent = normalize(cross(normalModel, vec3(tangent)) * tangent.w);

	tangentToModelMatrix = mat4(vec4(vec3(tangent), 0.0), vec4(bitangent, 0.0), vec4(normalModel, 0.0), vec4(pos, 1.0));

}