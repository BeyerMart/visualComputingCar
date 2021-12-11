#version 330 core

struct Material
{
    vec3 diffuse;
};

out vec4 fragColor;

uniform Material uMaterial;

void main(void)
{
    fragColor = vec4(uMaterial.diffuse, 1.0);
}
