#version 410 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 color;

uniform float time;
uniform mat4 proj;
uniform mat4 view;
flat out vec4 starColor;

void main(void)
{
	vec4 newVertex = position;

	newVertex.z += time;
	newVertex.z = fract(newVertex.z);

	float size = (20.0 * newVertex.z * newVertex.z);

	starColor = smoothstep(1.0, 7.0, size) * color;

	newVertex.z = (999.9 * newVertex.z) - 1000.0;
	gl_Position = proj * view * newVertex;
	gl_PointSize = size;
}