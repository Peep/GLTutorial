#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 vertexColor;
uniform vec3 ourColor;

void main()
{
    gl_Position = vec4(position, 1.0); // See how we directly give a vec3 to vec4's constructor
	vertexColor = color;
}