#version 430 core

uniform mat4 WorldInverse;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform int mode;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{

    // retrieve position from G-buffer
    vec3 worldPos = texture(gPosition, TexCoords).xyz;
    // retrieve other data from G-buffer
    vec3 N = normalize(texture(gNormal, TexCoords).xyz);
    vec3 specular = texture(gSpecular, TexCoords).rgb;
    float shininess = texture(gNormal, TexCoords).a;

    vec3 out_color = vec3(1, 1, 0);
    if (mode == 1)      out_color = vec3(TexCoords, 0);
    else if (mode == 2) out_color = texture(gPosition, TexCoords).rgb;
    else if (mode == 3) out_color = texture(gNormal, TexCoords).rgb;
    else if (mode == 4) out_color = texture(gDiffuse, TexCoords).rgb;
    else if (mode == 5) out_color = texture(gSpecular, TexCoords).rgb;
    else if (mode == 6) out_color = vec3(texture(gNormal, TexCoords).a / 128);


    FragColor = vec4(out_color, 1);
}
