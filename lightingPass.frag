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
    float Specular = AlbedoSpec.a * 128.0; // This was stored as matshinFS / 128.0

    // Ambient
    vec3 ambient = Albedo * 0.2; 
    
    // Lighting
    vec3 lightDir = normalize(lightPos - FragPos);
    
    // Diffuse
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diff * Albedo;
    
    // Specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Specular);
    vec3 specular = lightColor * spec * 0.5; // Simplified specular
    
    // Attenuation
    float dist = length(lightPos - FragPos);
    float attenuation = att / (1.0 + 0.5 * dist + 0.2 * (dist * dist));
    
    vec3 lighting = ambient + (diffuse + specular) * attenuation;
    
    FragColor = vec4(lighting, 1.0);
}
