#version 430 core

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
uniform mat4 model_inverse;
uniform vec3 view_position;
uniform vec3 light_position;

const float ambient_factor = 0.05f;

in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    // retrieve data from G-buffer
    vec3 worldPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords)).xyz;
    vec3 diffuse_color = texture(gDiffuse, TexCoords).rgb;
    vec3 specular_color = texture(gSpecular, TexCoords).rgb;
    float shininess = texture(gNormal, TexCoords).a;

    // Calculate lighting (Blinn-Phong)
    vec3 light_vector = light_position - worldPos;
    float distance = length(light_vector);
    vec3 lightDir   = normalize(light_vector);
    vec3 eye = (model_inverse*vec4(view_position, 0)).xyz;
    vec3 viewDir    = normalize(eye - worldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient  = diffuse_color * ambient_factor;
    vec3 diffuse  = diffuse_color * max(dot(normal, lightDir), 0.0);
    float spec_angle = max(dot(normal, halfwayDir), 0.0);
    vec3 specular = specular_color * pow(spec_angle, shininess*255);

    vec3 out_color = ambient + (diffuse + specular) * max(0, min(5000 / (distance * distance), 1-ambient_factor));

    FragColor = vec4(out_color * 2, 1);
}
