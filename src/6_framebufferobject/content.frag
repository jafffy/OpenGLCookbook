#version 430 core

in vec4 vsOutColor;

out vec4 outColor;

void main(void)
{
	outColor = vsOutColor;
}