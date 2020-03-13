#version 430 core

uniform mat4 WorldInverse;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform vec3 view_position;

const vec3 L = normalize(view_position);
const float ambient = 0.1f;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    // retrieve position from G-buffer
	vec3 worldPos = texture(gPosition, TexCoords).xyz;
	// retrieve other data from G-buffer
	vec3 N = normalize(texture(gNormal, TexCoords).xyz);
	vec3 Kd = texture(gDiffuse, TexCoords).rgb;
	vec3 specular = texture(gSpecular, TexCoords).rgb;
	float shininess = texture(gNormal, TexCoords).a;
	
	vec3 eye = (WorldInverse*vec4(0,0,0,1)).xyz;
	vec3 V = normalize(eye - worldPos);
	vec3 H = normalize(L+V);
	float NL = max(dot(N,L),0.0);
	float NV = max(dot(N,V),0.0);
	float HN = max(dot(H,N),0.0);

	vec3 out_color = Kd * ambient + Kd * NL + specular.rgb * pow(HN,shininess);
	
	FragColor = vec4(out_color, 1);
}
