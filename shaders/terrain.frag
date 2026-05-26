#version 330 core

out vec4 FragColor;

in float height;

void main()
{
    vec3 brown = vec3(0.45, 0.30, 0.15);
    vec3 green = vec3(0.2, 0.5, 0.2);
    vec3 gray = vec3(0.5, 0.5, 0.5);

    vec3 color;

    color = brown;

    FragColor = vec4(color, 1.0);
}