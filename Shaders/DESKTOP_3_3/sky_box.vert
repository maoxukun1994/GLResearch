#version 330 core

in vec4 vVertex;

uniform mat4 mvpMatrix;

out vec3 vVaryingTexCoord;

void main(void)
{
    vVaryingTexCoord = normalize(vVertex.xyz);

    vec4 pos = mvpMatrix * vVertex;

    gl_Position = pos.xyww;
}
