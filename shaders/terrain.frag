#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    vec3 norm = normalize(Normal);

    // directional-ish light
    vec3 lightDir = normalize(lightPos - FragPos);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);

    // slope factor
    float slope = 1.0 - norm.y;

    vec3 terrainColor = vec3(0.12, 0.35, 0.12);

    // ambient
    vec3 ambient = terrainColor * 0.15;

    // diffuse lighting
    vec3 diffuse = terrainColor * diff * 1.4;

    vec3 result = ambient + diffuse;

    FragColor = vec4(result, 1.0);
}