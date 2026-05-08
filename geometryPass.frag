#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec4 vertexSCO;
in vec3 normalSCO;
in vec2 texCoordFS;
in mat3 TBN;

in vec3 matdifFS;
in vec3 matspecFS;
in float matshinFS;

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform int hasTexture;
uniform int hasBumpTexture;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    gPosition = vertexSCO.xyz;
    
    // Also store the per-fragment normals into the gbuffer
    vec3 N;
    if (hasBumpTexture == 1)
    {
        vec3 tangentNormal = texture(bumpTex, texCoordFS).xyz * 2.0 - 1.0;
        N = normalize(TBN * tangentNormal);
    }
    else
    {
        N = normalize(normalSCO);
    }
    gNormal = N;

    // And the diffuse per-fragment color
    if (hasTexture == 1)
    {
        gAlbedoSpec.rgb = texture(diffuseTex, texCoordFS).rgb;
    }
    else
    {
        gAlbedoSpec.rgb = matdifFS;
    }
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = matshinFS / 128.0; 
}
