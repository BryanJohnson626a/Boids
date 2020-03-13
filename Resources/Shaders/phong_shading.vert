#version 430 core

uniform mat4 transform;

in vec3 in_position;
in vec3 in_normal;
in vec2 in_texcoord;

// Main function - entry Vector
void main()
{
  gl_Position = transform * vec4(in_position, 1);
}
