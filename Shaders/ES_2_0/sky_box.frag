precision mediump float;

varying vec3 vVaryingTexCoord;

uniform samplerCube cubeMap;

void main(void)
{
    gl_FragColor = textureCube(cubeMap,vVaryingTexCoord);
}
