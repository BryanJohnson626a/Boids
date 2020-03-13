#version 430 core

uniform vec3 view_position;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;
uniform vec3 object_color;


// Input from the rasterizer
in VS_OUT
{
	vec3 position;
	vec3 normal;
} fs_in;

// Output format to FBO
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec3 gDiffuse;
layout (location = 3) out vec3 gSpecular;

void main()
{
	gPosition = fs_in.position;
	gNormal = vec4(fs_in.normal, shininess);
	gDiffuse = diffuse;
	gSpecular = specular;
}
