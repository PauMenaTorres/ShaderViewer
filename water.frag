#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec4 vertexSCO;
in vec3 normalSCO;
in vec3 matdifFS;
in vec3 toCameraVector;

void main()
{
    gPosition = vertexSCO.xyz;
    gNormal = normalize(normalSCO);
    
    // Un color de agua básico por ahora
    vec3 waterColor = vec3(0.0, 0.4, 0.8);
    
    // Efecto Fresnel simple (opcional por ahora, lo dejaremos para más adelante,
    // pero ya pasamos el vector desde el vertex shader)
    vec3 viewVector = normalize(toCameraVector);
    float refractiveFactor = dot(viewVector, gNormal);
    
    // Mezclamos un poco con el color difuso del material
    gAlbedoSpec.rgb = mix(matdifFS, waterColor, 0.8); 
    
    // Alto brillo especular (0 a 1) para que el agua parezca húmeda/reflectante
    gAlbedoSpec.a = 0.9; 
}
