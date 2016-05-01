#version 430 core

in vec2 UV;

out vec4 outColor;

uniform sampler2D renderedTextureSampler;

void main(void)
{
	outColor = texture(renderedTextureSampler, UV);
}
