#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

uniform vec3 lightPos; // En SCO
uniform vec3 lightColor;
uniform float att;

void main()
{             
    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec4 AlbedoSpec = texture(gAlbedoSpec, TexCoords);
    vec3 Albedo = AlbedoSpec.rgb;
    float SpecularVal = AlbedoSpec.a * 128.0;
    bool isWater = false;
    if (SpecularVal < 0.0)
    {
        isWater = true;
        SpecularVal = -SpecularVal;
    }

    // If it is a background pixel (no geometry normal), render the clear albedo color (sky color) directly
    if (length(Normal) < 0.1)
    {
        FragColor = vec4(0.25, 0.61, 0.9, 1.0); // Beautiful sky-blue background color
        return;
    }

    // Lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    
    // Attenuation
    float dist = length(lightPos - FragPos);
    float attenuation = att / (1.0 + 0.5 * dist + 0.2 * (dist * dist));

    if (isWater)
    {
        // Water specular highlight (glisten)
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), SpecularVal);
        vec3 specular = lightColor * spec * 0.8; // High specular factor for extra glistening water!
        
        // Blend final color: Mixed reflection/refraction albedo remains crisp and bright,
        // with the specular light glisten added dynamically on top!
        vec3 lighting = Albedo + specular * attenuation;
        FragColor = vec4(lighting, 1.0);
        return;
    }

    // Ambient
    vec3 ambient = Albedo * 0.2; 
    
    // Diffuse
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diff * Albedo;
    
    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), SpecularVal);
    vec3 specular = lightColor * spec * 0.5; // Simplified specular for standard models
    
    vec3 lighting = ambient + (diffuse + specular) * attenuation;
    FragColor = vec4(lighting, 1.0);
}
