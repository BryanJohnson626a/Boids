#version 430 core

uniform mat4 transform;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 3) in mat4 transform_instance;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
  vec3 position;
  vec3 normal;
} vs_out;

void main()
{
  mat4 final_transform = transform * transform_instance;
  gl_Position = final_transform * vec4(position, 1);
  vs_out.position = vec3(transform_instance * vec4(position, 1));
  vs_out.normal = mat3(transform_instance) * normal;
}
