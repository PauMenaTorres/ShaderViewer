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
uniform vec2 viewportSize;   // dynamic viewport resolution

uniform float waveStrength = 0.02;     // customizable wave amplitude/strength
uniform float waterShininess = 32.0;   // customizable specular shininess (damper)

void main()
{
    // 1. Calculate projective coordinates (screen space to NDC)
    vec2 ndc = gl_FragCoord.xy / viewportSize;
    
    vec2 refractTexCoords = vec2(ndc.x, ndc.y);
    vec2 reflectTexCoords = vec2(ndc.x, ndc.y); // No flip needed when using a mathematically reflected view matrix!
    
    // 2. Sample DuDv map twice to create overlapping organic wave distortions
    vec2 distortedTexCoords = texture(dudvMap, vec2(texCoordsFS.x + moveFactor, texCoordsFS.y)).rg * 0.1;
    distortedTexCoords = texCoordsFS + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
    
    vec2 distortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
    
    // Apply distortion
    refractTexCoords = clamp(refractTexCoords + distortion, 0.001, 0.999);
    reflectTexCoords.x = clamp(reflectTexCoords.x + distortion.x, 0.001, 0.999);
    reflectTexCoords.y = clamp(reflectTexCoords.y + distortion.y, 0.001, 0.999);

    // 3. Normal mapping
    vec3 normalColor = texture(normalMap, distortedTexCoords).rgb;
    vec3 normalTangent = normalize(normalColor * 2.0 - 1.0);
    
    // Smooth out normal perturbation for cleaner waves
    normalTangent.xy *= 0.6;
    normalTangent = normalize(normalTangent);
    
    vec3 perturbedNormalSCO = normalize(TBN * normalTangent);

    // 4. Fresnel effect
    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, perturbedNormalSCO);
    refractiveFactor = clamp(pow(refractiveFactor, 3.0), 0.15, 0.85); // Tuned to make reflection highly dominant and clear!

    // 5. Sample FBO reflection/refraction colors
    vec3 reflectColour = texture(reflectionTexture, reflectTexCoords).rgb;
    vec3 refractColour = texture(refractionTexture, refractTexCoords).rgb;

    // 6. Blend colors and apply characteristic water tint
    vec3 finalColor = mix(reflectColour, refractColour, refractiveFactor);
    vec3 waterTint = vec3(0.02, 0.25, 0.45); // Sleek deep blue-green aqua color
    finalColor = mix(finalColor, waterTint, 0.15); // Low mix factor to keep reflection colors crisp and visible!

    // Write attributes to the Deferred G-Buffer
    gPosition = vertexSCO.xyz;
    gNormal = perturbedNormalSCO;
    gAlbedoSpec.rgb = finalColor;
    
    // Pass high shininess (Specular intensity) in alpha channel for deferred specular lighting!
    gAlbedoSpec.a = -waterShininess / 128.0; 
}
