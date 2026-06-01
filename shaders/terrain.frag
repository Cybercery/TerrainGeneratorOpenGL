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


    // normalize terrain height
    float h = clamp(FragPos.y / 80.0 , 0.0, 1.0);

    vec3 grass = vec3(34.0, 139.0, 34.0) / 255.0;
    vec3 dirt  = vec3(120.0, 72.0, 48.0) / 255.0;
   // vec3 dirt = vec3(0.0, 100.0, 0.0) / 255.0; // dark green test
    vec3 rock  = vec3(110.0, 110.0, 110.0) / 255.0;
    vec3 snow  = vec3(1.0);

    vec3 terrainColor = grass;

    terrainColor = mix(
        terrainColor,
        dirt,
        smoothstep(0.15, 0.65, h)
    );

    terrainColor = mix(
        terrainColor,
        rock,
        smoothstep(0.65, 0.75, h)
    );

    terrainColor = mix(
        terrainColor,
        snow,
        smoothstep(0.75, 1.0, h)
    );

    // ambient
    vec3 ambient = terrainColor * 0.5;

    // diffuse lighting
    vec3 diffuse = terrainColor * diff;

    vec3 result = ambient + diffuse;

    FragColor = vec4(result, 1.0);
}