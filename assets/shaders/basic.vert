#version 430
layout (location = 0) in vec3 aPos;
layout (location = 2) uniform mat4 model;
layout (location = 3) uniform mat4 view;
layout (location = 4) uniform mat4 projection;
void main() {
	//gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_Position = vec4(aPos, 1.0);
}