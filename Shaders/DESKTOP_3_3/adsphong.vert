#version 330 core

in vec4 vVertex;
in vec3 vNormal;
in vec2 vTexCoords;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 vLightPosition;

smooth out vec3 vVaryingNormal;
smooth out vec3 vVaryingLightDir;
smooth out vec2 vVaryingTexCoords;
smooth out vec3 vPos;

void main(void)
{
    vec4 vPosition4 = mvMatrix * vVertex;
    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;

    vPos = vPosition3;
    vVaryingNormal = normalMatrix * vNormal;
    vVaryingLightDir = normalize(vLightPosition - vPosition3);
    vVaryingTexCoords = vTexCoords;

    gl_Position = mvpMatrix * vVertex;
}
