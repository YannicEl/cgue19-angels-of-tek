#version 430
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;
uniform mat3 normalMatrix;

out VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

void main() {
	vert.uv = uv;
	vert.normal_world = normalMatrix * normal;
	// wie wach is das eig
	vert.position_world = vec4(modelMatrix * vec4(position, 1)).xyz;

	gl_Position = viewProjMatrix * modelMatrix * vec4(position, 1.0);
}