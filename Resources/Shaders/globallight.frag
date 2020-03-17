#version 430 core

uniform mat4 WorldInverse;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform vec3 view_position;

const float ambient = 0.15f;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    // retrieve data from G-buffer
    vec3 worldPos = texture(gPosition, TexCoords).xyz;
    vec3 N = normalize(texture(gNormal, TexCoords).xyz);
    vec3 Kd = texture(gDiffuse, TexCoords).rgb;
    vec3 specular = texture(gSpecular, TexCoords).rgb;
    float shininess = texture(gNormal, TexCoords).a;

    // Calculate lighting (Blinn-Phong
    vec3 L = view_position - worldPos;
    float distance = pow(length(L), 1.5);
    L = normalize(L);
    vec3 eye = (WorldInverse*vec4(view_position, 0)).xyz;
    vec3 V = normalize(eye - worldPos);
    vec3 H = normalize(L+V);
    float NL = clamp(dot(N, L), 0, 1);
    float NV = clamp(dot(N, V), 0, 1);
    float HN = clamp(dot(H, N), 0, 1);
    float distance_factor = clamp(100 / distance,0,1);

    vec3 out_color = Kd * ambient + (Kd * NL + specular * pow(HN, shininess*255)) * distance_factor;

    FragColor = vec4(out_color, 1);
}
