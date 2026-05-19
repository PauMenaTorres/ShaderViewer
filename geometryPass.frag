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

// Dynamic lighting parameters (in Camera Space / SCO)
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float att;
uniform bool isReflectionOrRefraction = false;

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

    // If rendering to reflection/refraction FBOs (non-G-Buffer), compute full shading on-the-fly!
    if (isReflectionOrRefraction)
    {
        // 1. Ambient
        vec3 ambient = gAlbedoSpec.rgb * 0.2;

        // 2. Diffuse lighting
        vec3 lightDir = normalize(lightPos - vertexSCO.xyz);
        float diff = max(dot(N, lightDir), 0.0);
        vec3 diffuse = lightColor * diff * gAlbedoSpec.rgb;

        // 3. Specular highlight
        vec3 viewDir = normalize(-vertexSCO.xyz);
        vec3 reflectDir = reflect(-lightDir, N);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), matshinFS);
        vec3 specular = lightColor * spec * 0.5;

        // 4. Attenuation
        float dist = length(lightPos - vertexSCO.xyz);
        float attenuation = att / (1.0 + 0.5 * dist + 0.2 * (dist * dist));

        // Overwrite raw albedo with fully lit pixel color for the FBO texture
        gAlbedoSpec.rgb = ambient + (diffuse + specular) * attenuation;
    }
}
