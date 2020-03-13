#version 430 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(in_position, 1);
    TexCoords = in_texcoord;
}
