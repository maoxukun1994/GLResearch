#version 330 core

in vec3 vVaryingTexCoord;

out vec4 vFragColor;

uniform samplerCube cubeMap;

void main(void)
{
    vFragColor = texture(cubeMap,vVaryingTexCoord);
}
