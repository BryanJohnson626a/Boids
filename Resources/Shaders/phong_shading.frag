#version 430 core

uniform vec4 object_color;

// Output data
out vec4 color;

void main()
{
	color = object_color;
}
