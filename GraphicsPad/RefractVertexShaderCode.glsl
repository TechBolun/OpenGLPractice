#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) vec3 normalModel;
in layout(location=3) vec2 vertexTexCoord;
in layout(location=4) vec4 tangent;

uniform mat4 reflectionTransformMatrix;
uniform mat4 reflectionModelToWorldMatrix;

out vec3 theColor;
out vec2 uv;
out vec3 worldPos;
out vec3 FragNormal;
out mat4 tangentToModelMatrix;

void main()
{
	vec4 pos = vec4(position,1.0);

	gl_Position = reflectionTransformMatrix * pos;

	worldPos = vec3(reflectionModelToWorldMatrix * vec4(position, 1.0));

	uv = vertexTexCoord;

	vec3 bitangent = normalize(cross(normalModel, vec3(tangent)) * tangent.w);

	tangentToModelMatrix = mat4(vec4(vec3(tangent), 0.0), vec4(bitangent, 0.0), vec4(normalModel, 0.0), vec4(position, 1.0));

	FragNormal = normalModel;

	theColor = vertexColor;
}