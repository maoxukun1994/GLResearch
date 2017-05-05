attribute vec4 vVertex;
attribute vec3 vNormal;
attribute vec2 vTexCoords;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
uniform vec3 vLightPosition;

varying vec3 vVaryingNormal;
varying vec3 vVaryingLightDir;
varying vec2 vVaryingTexCoords;
varying vec3 vPos;

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
