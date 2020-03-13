#version 430 core


uniform mat4 transform;
uniform mat4 model_transform;
uniform vec3 view_position;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

// Interpolating vertex attributes over the rasterizer
out VS_OUT
{
  vec3 position;
  vec3 normal;
} vs_out;

void main()
{
  gl_Position = transform * vec4(position, 1);

  vs_out.position = vec3(model_transform * vec4(position, 1));
  vs_out.normal = vec3(model_transform * vec4(normal, 0));
}
