#version 430 core

layout (location = 0) in vec3 position;

uniform samplerBuffer Ms;
uniform mat4 VP;

out vec4 vsOutColor;

void main(void)
{
	vec4 col0 = texelFetch(Ms, 4 * gl_InstanceID + 0);
	vec4 col1 = texelFetch(Ms, 4 * gl_InstanceID + 1);
	vec4 col2 = texelFetch(Ms, 4 * gl_InstanceID + 2);
	vec4 col3 = texelFetch(Ms, 4 * gl_InstanceID + 3);

	mat4 M = mat4(col0, col1, col2, col3);

	gl_Position = VP * M * vec4(position, 1.0);

	vec4 colors[4] = {
		vec4(1.0, 0.0, 0.0, 0.5),
		vec4(0.0, 1.0, 0.0, 0.5),
		vec4(0.0, 0.0, 1.0, 0.5),
		vec4(1.0, 1.0, 1.0, 0.5),
	};

	vsOutColor = colors[gl_InstanceID % 4];
}