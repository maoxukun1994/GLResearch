precision mediump float;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform sampler2D colorMap;

varying vec3 vVaryingNormal;
varying vec3 vVaryingLightDir;
varying vec2 vVaryingTexCoords;
varying vec3 vPos;

void main(void)
{

    vec3 fogColor = vec3(0.1,0.1,0.15);
    float dist = length( vPos.xyz );
    float fogFactor = (100.0 - dist) / (100.0 - 20.0);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );



    vec3 ads;
    float diff = max(0.0,dot(normalize(vVaryingNormal),normalize(vVaryingLightDir)));
    ads = fogFactor * diff * diffuseColor.rgb;
    ads += ambientColor.rgb;
    ads *= texture2D(colorMap,vVaryingTexCoords).rgb;
    vec3 vReflection = normalize(reflect(-normalize(vVaryingLightDir),normalize(vVaryingNormal)));
    float spec = max(0.0,dot(normalize(vVaryingNormal),vReflection)) ;
    if(diff != 0.0)
    {
        float fSpec = pow(spec,32.0) * fogFactor;
        ads.rgb += vec3(fSpec,fSpec,fSpec);
    }

    vec3 color = mix( fogColor, ads, fogFactor);

    gl_FragColor = vec4(color,1.0);
}
