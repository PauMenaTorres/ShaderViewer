#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec4 vertexSCO;
in vec3 normalSCO;
in vec3 matdifFS;
in vec3 toCameraVector;
in vec4 clipSpaceCoords;
in vec2 texCoordsFS;
in mat3 TBN;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform float moveFactor;
uniform vec3 cameraPosition; // in world space

const float WAVE_STRENGTH = 0.02;

void main()
{
    // 1. Calculate projective coordinates (clip space to NDC)
    vec2 ndc = (clipSpaceCoords.xy / clipSpaceCoords.w) / 2.0 + 0.5;
    
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, 1.0 - ndc.y); // Flip Y for reflection texture mapping

    // 2. Sample DuDv map twice to create overlapping organic wave distortions
    vec2 distortedTexCoords = texture(dudvMap, vec2(texCoordsFS.x + moveFactor, texCoordsFS.y)).rg * 0.1;
    distortedTexCoords = texCoordsFS + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
    
    vec2 distortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * WAVE_STRENGTH;
    
    // Apply distortion
    refractTexCoords = clamp(refractTexCoords + distortion, 0.001, 0.999);
    reflectTexCoords.x = clamp(reflectTexCoords.x + distortion.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y + distortion.y, 0.001, 0.999);

    // 3. Sample normal map & perturb normal in SCO
    vec4 normalColor = texture(normalMap, distortedTexCoords);
    vec3 tangentNormal = vec3(
        normalColor.r * 2.0 - 1.0,
        normalColor.b * 3.0,          // Amplified Y component (upward normal of plane)
        normalColor.g * 2.0 - 1.0
    );
    vec3 perturbedNormalSCO = normalize(TBN * tangentNormal);

    // 4. Fresnel effect
    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, perturbedNormalSCO);
    refractiveFactor = clamp(pow(refractiveFactor, 0.6), 0.0, 1.0); // pow factor for aesthetic tuning

    // 5. Sample FBO reflection/refraction colors
    vec3 reflectColour = texture(reflectionTexture, reflectTexCoords).rgb;
    vec3 refractColour = texture(refractionTexture, refractTexCoords).rgb;

    // 6. Blend colors and apply characteristic water tint
    vec3 finalColor = mix(reflectColour, refractColour, refractiveFactor);
    vec3 waterTint = vec3(0.05, 0.35, 0.55); // Rich deep blue-green aqua color
    finalColor = mix(finalColor, waterTint, 0.25);

    // Write attributes to the Deferred G-Buffer
    gPosition = vertexSCO.xyz;
    gNormal = perturbedNormalSCO;
    gAlbedoSpec.rgb = finalColor;
    
    // Pass high shininess (Specular intensity) in alpha channel for deferred specular lighting!
    // We map a specular damper of 32.0, so: shininess = 32.0 / 128.0 = 0.25
    gAlbedoSpec.a = 32.0 / 128.0; 
}
