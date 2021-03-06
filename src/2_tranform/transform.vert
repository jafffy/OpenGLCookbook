#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 M;

void main(void)
{
	gl_Position = M * vec4(position, 1.0);
}
