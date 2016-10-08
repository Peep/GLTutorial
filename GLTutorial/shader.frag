#version 410 core

uniform vec3 ourColor;

out vec4 color;
in vec3 vertexColor;
  
void main()
{
	vec3 result = vertexColor;
    color = vec4(result, 1.0f);
}