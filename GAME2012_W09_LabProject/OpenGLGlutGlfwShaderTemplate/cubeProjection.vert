#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(vertex_position, 1.0);
	color = vertex_color;
}